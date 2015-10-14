#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <hw/hw.h>
#include <hw/sdr.h>

#define SAMPLE_RATE   1000000
#define RESAMPLE_RATE 50000

#define CENTER_FREQ   446500000
#define FFT_LEVEL     10
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

int main( int argc, char **argv )
{

	int c;
	int ret;

	uint8_t *buf,    *sample_buf;
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
	ret != dongle_set_agc( dongle, 40 );
	if (ret != 0)
	{
		printf("Cannot properly config device\n");
	}

	sample_len = SAMPLE_LENGHT;
	sample_buf = malloc( sample_len );

	while ( 1 )
	{
		//read plain samples
		dongle_read_samples( dongle, sample_buf, sample_len );
		//delay
		//rotate by 90 degrees
		rotate_90( sample_buf, sample_len );
		//downsample
		//sample rate 1Mhz downsample to 50Khz 

		//play
		
		usleep(10000);
		//printf("Sample\n");
	}

main_exit:
	sdr_close(0);

	return 0;
}