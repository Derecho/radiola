#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <complex.h>

#include <hw/hw.h>
#include <hw/sdr.h>

#define SAMPLE_RATE   1000000
#define RESAMPLE_RATE 50000

#define CENTER_FREQ   101100000
#define FFT_LEVEL     18
#define FFT_SIZE      (1 << FFT_LEVEL)
#define SAMPLE_LENGHT (2 * FFT_SIZE)
#define PRESCALE      8
#define POSTSCALE     2


void helper( char *exec_name )
{
	const char help_str[]="Usage: ./%s [OPTIONS]\n\
-f [FREQ]    - set center frequency\n\
-s [SAMPLE]  - set sample rate\n\
-d [DEVICE]  - choose device\n\
-r [RESAMPE] - set resample rate\n\
-? - print help\n\
";
	printf( help_str, exec_name );
}

unsigned int super_atan( signed int x1, signed int y1 )
{
   // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
   // Converts any XY values including 0 to a degree value that should be
   // within +/- 1 degree of the accurate value without needing
   // large slow trig functions like ArcTan() or ArcCos().
   // NOTE! at least one of the X or Y values must be non-zero!
   // This is the full version, for all 4 quadrants and will generate
   // the angle in integer degrees from 0-360.
   // Any values of X and Y are usable including negative values provided
   // they are between -1456 and 1456 so the 16bit multiply does not overflow.

   unsigned char negflag;
   unsigned char tempdegree;
   unsigned char comp;
   unsigned int degree;     // this will hold the result
   signed int x=x1;            // these hold the XY vector at the start
   signed int y=y1;            // (and they will be destroyed)
   unsigned int ux;
   unsigned int uy;

   // Save the sign flags then remove signs and get XY as unsigned ints
   negflag = 0;
   if(x < 0)
   {
      negflag += 0x01;    // x flag bit
      x = (0 - x);        // is now +
   }
   ux = x;                // copy to unsigned var before multiply
   if(y < 0)
   {
      negflag += 0x02;    // y flag bit
      y = (0 - y);        // is now +
   }
   uy = y;                // copy to unsigned var before multiply

   // 1. Calc the scaled "degrees"
   if(ux > uy)
   {
      degree = (uy * 45) / ux;   // degree result will be 0-45 range
      negflag += 0x10;    // octant flag bit
   }
   else
   {
      degree = (ux * 45) / uy;   // degree result will be 0-45 range
   }

   // 2. Compensate for the 4 degree error curve
   comp = 0;
   tempdegree = degree;    // use an unsigned char for speed!
   if(tempdegree > 22)      // if top half of range
   {
      if(tempdegree <= 44) comp++;
      if(tempdegree <= 41) comp++;
      if(tempdegree <= 37) comp++;
      if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
   }
   else    // else is lower half of range
   {
      if(tempdegree >= 2) comp++;
      if(tempdegree >= 6) comp++;
      if(tempdegree >= 10) comp++;
      if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
   }
   degree += comp;   // degree is now accurate to +/- 1 degree!

   // Invert degree if it was X>Y octant, makes 0-45 into 90-45
   if(negflag & 0x10) degree = (90 - degree);

   // 3. Degree is now 0-90 range for this quadrant,
   // need to invert it for whichever quadrant it was in
   if(negflag & 0x02)   // if -Y
   {
      if(negflag & 0x01)   // if -Y -X
            degree = (180 + degree);
      else        // else is -Y +X
            degree = (180 - degree);
   }
   else    // else is +Y
   {
      if(negflag & 0x01)   // if +Y -X
            degree = (360 - degree);
   }
   return degree;
}

/*
Page about div
http://bisqwit.iki.fi/story/howto/bitmath/#DivAndModDivisionAndModulo
*/

uint8_t super_div20u8( uint16_t num )
{
	uint16_t a = num;
	uint16_t b = 20;
	uint16_t r = 0;
	uint16_t m = 1;
	while ( b < a )
	{
		b = b << 1;
		m = m << 1;
	}
	do 
	{
		if ( a >= b )
		{
			a = a - b;
			r = r + m;
		}
		b = b >> 1;
		m = m >> 1;
	} while ( m != 0 );
	return (uint8_t)r;
}

void rotate_90(uint8_t *buf, uint32_t len)
/* 90 rotation is 1+0j, 0+1j, -1+0j, 0-1j
   or [0, 1, -3, 2, -4, -5, 7, -6] */
{
	uint32_t i;
	uint8_t tmp;
	for (i=0; i<len; i+=8) {
		/* uint8_t negation = 255 - x */
		tmp = 255 - buf[i+3];
		buf[i+3] = buf[i+2];
		buf[i+2] = tmp;

		buf[i+4] = 255 - buf[i+4];
		buf[i+5] = 255 - buf[i+5];

		tmp = 255 - buf[i+6];
		buf[i+6] = buf[i+7];
		buf[i+7] = tmp;
	}
}

float to_float(uint8_t x) {
    return (1.0f/127.0f)*(((float)x)-127.0f);
}

//float ph_ch( uint8_t i1, uint8_t q1, uint8_t i2, uint8_t q2)
float ph_ch( uint8_t i1, uint8_t q1 )
{
	static float complex last=CMPLXF(0.0f, 0.0f);
	float out;
	float complex xy,c1;
	//float c2;

	//c1 = to_float(i1) + I*to_float(q1);
	c1 = CMPLXF( to_float(i1), to_float(q1) );
	//c2 = to_float(i2) + I*to_float(q2);
	//c2 = CMPLXF( to_float(i2), to_float(q2) );
	xy = conjf(last)*c1;
	out = cargf( xy );
	last = c1;

	return out;
}

//delay filtering
void delay_filt( uint8_t *buf, int buf_len )
{
	//delay length
	const int n=10;
	
	int i=0;
	uint32_t cycle=0;
	uint32_t avg_i=0, avg_q=0;
	uint32_t cyc_buffer_i[n],delay_i=0;
	uint32_t cyc_buffer_q[n],delay_q=0;
	uint8_t in1=0,in2=0,out1=0,out2=0;

	memset( cyc_buffer_i, 0, n*sizeof(uint32_t) );
	memset( cyc_buffer_q, 0, n*sizeof(uint32_t) );

	//for (i=0; i<(buf_len-(n*2));i+=2)
	for (i=0 ; i<(buf_len-1) ; i+=2 )
	//for (i=0; i<1000; i+=2)
	{
		in1 = buf[i];
		in2 = buf[i+1];

		//average
		avg_i += (uint32_t)in1;
		avg_q += (uint32_t)in2;

		delay_i = cyc_buffer_i[cycle];
		delay_q = cyc_buffer_q[cycle];
		
		cyc_buffer_i[cycle] = avg_i;
		cyc_buffer_q[cycle] = avg_q;
		cycle = cycle + 1;
		//cycle += 1;
		if ( cycle >= n )
		{
			cycle = 0;
		}

		out1 = (avg_i - delay_i)/n;
		out2 = (avg_q - delay_q)/n;

		buf[i]   = out1;
		buf[i+1] = out2;

		//printf("%d,avg=[%d,%d],delay=[%d,%d],in=[%d,%d],out=[%d,%d]\n",
		//	cycle, avg_i,avg_q,delay_i,delay_q,in1,in2,out1,out2);

	}
}

int main( int argc, char **argv )
{

	int c;
	int ret;
	int i,j,m;

	uint8_t *buf, *sample_buf;
	float	*fbuf;
	signed short *sound_buf;
	int 	buf_len, sample_len;

	//commandline config params
	int      config_device        = 0;
	uint32_t config_freq          = CENTER_FREQ;
	uint32_t config_sample_rate   = SAMPLE_RATE;
	uint32_t config_resample_rate = RESAMPLE_RATE;

	sdr_t *sdr = NULL;
	dongle_t *dongle = NULL;

	// get all argument configs
	opterr = 0;
	while ( (c = getopt(argc, argv, "f:s:d:")) != -1 )
	{
		switch ( c )
		{
			case 'f':
				config_freq = atoi( optarg );
				break;
			case 's':
				config_sample_rate = atoi( optarg );
				break;
			case 'd':
				//printf(" %s \n", optarg);
				config_device = atoi( optarg );
				break;
			case '?':
				helper( argv[0] );
				break;
			case ':':
				printf("\n");
				break;
			default:
				printf("Unknow option\n");
				return -1;
		}
	}

	if ( (sdr = sdr_init()) == NULL )
	{		
		printf("Cannot init sdr manager\n");
		sdr = NULL;
		goto main_exit;
	}

	if ( sdr_open_device( sdr, config_device ) != 0 )
	{
		printf("MAIN:Cannot open device %d\n", config_device);
		sdr->dongle  = NULL;
		goto main_exit;
	}

	dongle = sdr_get_device_id( sdr, config_device );
	ret = 0;
	ret != dongle_set_freq( dongle, config_freq );
	ret != dongle_set_sample_rate( dongle, config_sample_rate );
	ret != dongle_set_gain( dongle, 0 );
	ret != dongle_set_agc( dongle, 10 );
	if (ret != 0)
	{
		printf("Cannot properly config device\n");
	}

	sample_len = SAMPLE_LENGHT;
	sample_buf = malloc( SAMPLE_LENGHT );
	fbuf = malloc( SAMPLE_LENGHT*sizeof(float) );
	sound_buf = malloc( (SAMPLE_LENGHT/20)*sizeof(signed short) );

	while ( 1 )
	{
		//read plain samples
		dongle_read_samples( dongle, sample_buf, SAMPLE_LENGHT );
		//convert data to float
		//for ( i=0; i<sample_len; i++)
		//{
		//	fbuf[i] = to_float(sample_buf[i]);
		//}
		//delay boxed filter
		delay_filt( sample_buf, sample_len );


		//rotate by 90 degrees uint8_t
		//rotate_90( sample_buf, sample_len );

		//with unsigned data phace change
		#if 0
		for (i=0,j=0; i<sample_len-2; i+=2,j++)
		{
			uint8_t c[2];
			uint8_t *a=&sample_buf[i], *b=&sample_buf[i+2];
			double angle;
			float pcm;

			c[0] = a[0]*b[0] - a[1]*b[1];
			c[1] = a[1]*b[0] + a[0]*b[1];
			angle = atan2((double)c[1],(double)c[0]);
			pcm = (int)(angle / 3.14159 * (1<<14));
			fbuf[j] = pcm;
		}
		#endif
		//phase change with float
		for (i=0; i<SAMPLE_LENGHT-4; i+=2)
		{
			//fbuf[i] = ph_ch( sample_buf[i], sample_buf[i+1], 
			//	sample_buf[i+2], sample_buf[i+3] );
			fbuf[i] = ph_ch( sample_buf[i], sample_buf[i+1]);
		}

		//downsample
		//sample rate 1Mhz downsample to 50Khz 
		for ( i=0,m=0; i<SAMPLE_LENGHT; i+=20, m+=1 )
		{
			float sum=0.0;
			for (j=i;j<i+20;j++)
			{
				sum += fbuf[j];
			}
			//sound_buf[m] = (signed short)(10000.0f*(sum/20.0));
			sound_buf[m] = (signed short)(10000.0f*(sum/20.0));
		}
		

		//write to play
		write(1, sound_buf, ((SAMPLE_LENGHT/20)*sizeof(signed short)));
		//exit(0);
		//usleep(1);
		//printf("Sample\n");
	}

main_exit:
	sdr_close(0);

	return 0;
}