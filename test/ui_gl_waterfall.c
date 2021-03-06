#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

//radiola
#include <draw/glui.h>
#include <hw/hw.h>
#include <hw/sdr.h>

#define SAMPLE_RATE  2048000

#define CENTER_FREQ  446500000
#define FFT_LEVEL    10
#define FFT_SIZE     (1 << FFT_LEVEL)
#define SAMPLE_LENGHT   (2 * FFT_SIZE)
#define PRESCALE     8
#define POSTSCALE    2

int16_t* Sinewave;
int N_WAVE, LOG2_N_WAVE;
double* power_table;

void helper( char *exec_name )
{
	const char help_str[]="Usage: ./%s [OPTIONS]\n\
-f [FREQ] - set center frequency\n\
-s [SAMPLE] - set sample rate\n\
-d [DEVICE] - choose device\n\
-? - print help\n\
";
	printf( help_str, exec_name );
}

//better to have size  size mod olen == 0
int normalise( uint8_t *ibuf, int ilen, uint8_t *obuf, int olen )
{
	int i,j,m;
	int ppi;

	/*
	if ( ilen >= olen )
	{
		ppi = ilen / olen;
	} else {
		return -1;
	}
	*/
	ppi = 1;
	m = 0;
	i = 0;
	while ( (i < ilen) && (m < olen) )
	{
		uint32_t sum=0;
		for ( j=0; j<ppi; j++ );
			sum += ibuf[i+j];
		sum /= ppi;
		obuf[m] = sum;
		//printf("%d-", obuf[m] );

		i += ppi;
		m += 1;
	}

	return 0;
}

void sine_table(int size)
{
    int i;
    double d;
    LOG2_N_WAVE = size;
    N_WAVE = 1 << LOG2_N_WAVE;
    Sinewave = malloc(sizeof(int16_t) * N_WAVE*3/4);
    power_table = malloc(sizeof(double) * N_WAVE);
    for (i=0; i<N_WAVE*3/4; i++)
    {
        d = (double)i * 2.0 * M_PI / N_WAVE;
        Sinewave[i] = (int)round(32767*sin(d));
    }
}

int16_t FIX_MPY(int16_t a, int16_t b)
/* fixed point multiply and scale */
{
	int c = ((int)a * (int)b) >> 14;
	b = c & 0x01;
	return (c >> 1) + b;
}

int32_t real_conj(int16_t real, int16_t imag)
/* real(n * conj(n)) */
{
    return ((int32_t)real*(int32_t)real + (int32_t)imag*(int32_t)imag);
}

int fix_fft(int16_t iq[], int m)
/* interleaved iq[], 0 <= n < 2**m, changes in place */
{
	int mr, nn, i, j, l, k, istep, n, shift;
	int16_t qr, qi, tr, ti, wr, wi;
	n = 1 << m;
	if (n > N_WAVE)
		{return -1;}
	mr = 0;
	nn = n - 1;
	/* decimation in time - re-order data */
	for (m=1; m<=nn; ++m) {
		l = n;
		do
			{l >>= 1;}
		while (mr+l > nn);
		mr = (mr & (l-1)) + l;
		if (mr <= m)
			{continue;}
		// real = 2*m, imag = 2*m+1
		tr = iq[2*m];
		iq[2*m] = iq[2*mr];
		iq[2*mr] = tr;
		ti = iq[2*m+1];
		iq[2*m+1] = iq[2*mr+1];
		iq[2*mr+1] = ti;
	}
	l = 1;
	k = LOG2_N_WAVE-1;
	while (l < n) {
		shift = 1;
		istep = l << 1;
		for (m=0; m<l; ++m) {
			j = m << k;
			wr =  Sinewave[j+N_WAVE/4];
			wi = -Sinewave[j];
			if (shift) {
				wr >>= 1; wi >>= 1;}
			for (i=m; i<n; i+=istep) {
				j = i + l;
				tr = FIX_MPY(wr,iq[2*j]) - FIX_MPY(wi,iq[2*j+1]);
				ti = FIX_MPY(wr,iq[2*j+1]) + FIX_MPY(wi,iq[2*j]);
				qr = iq[2*i];
				qi = iq[2*i+1];
				if (shift) {
					qr >>= 1; qi >>= 1;}
				iq[2*j] = qr - tr;
				iq[2*j+1] = qi - ti;
				iq[2*i] = qr + tr;
				iq[2*i+1] = qi + ti;
			}
		}
		--k;
		l = istep;
	}
	return 0;
}

//fftize
int simple_fft( uint8_t *buf, int len )
{
	int i,j;
	uint16_t p;
	uint16_t buf1[SAMPLE_LENGHT];
	uint16_t buf2[SAMPLE_LENGHT];
	int fft_len;

	for (i=0; i<len; i++)
	{
		buf1[i] = buf[i] * PRESCALE;
	}

	fix_fft( (uint16_t *)buf1, FFT_LEVEL );

	for (i=0; i<FFT_SIZE; i+=1)
	{
		//buf1[i] = rtl_out.buf[i];
		//p = buf1[i] * buf1[i];
		j = i*2;
		p = (int16_t)real_conj(buf1[j], buf1[j + 1]);
		buf2[i] = p;
	}

	fft_len = FFT_SIZE / 2;
	for (i=0; i<fft_len; i++)
	{
		buf[i] =         (int)log10(POSTSCALE * (float)buf2[i+fft_len]);
		buf[i+fft_len] = (int)log10(POSTSCALE * (float)buf2[i]);
	}

	return 0;
}

int main( int argc, char **argv )
{

	int ret;
	int i,j;
	int c;
	uint8_t *buf, *sample_buf;
	int buf_len, sample_len;
	uint32_t dev_num;
	//config params
	int      config_device      = 0;
	uint32_t config_freq        = CENTER_FREQ;
	//int      config_gain        = 1;
	//int      config_agc         = 1;
	uint32_t config_sample_rate = SAMPLE_RATE;
	char *endptr = NULL;


	sdr_t *sdr = NULL;
	dongle_t *dongle = NULL;


	glui_t *t = NULL;
	glui_waterfall_t *w = NULL;

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

	sine_table( FFT_LEVEL );


	//printf("%x\n",t);
	//open GUI
	if ( glui_init( &t ) == -1 )
	{
		printf("Cannot set glui\n");
		return 1;
	}

	//printf("%x\n",t);
	if ( glui_waterfall( &t, &w ) == -1 )
	{
		printf("Cannot set waterfall\n");
		return 1;
	}
	
	/*
	dev_num = rtlsdr_get_device_count();
	if ( dev_num < 1 )
	{
		printf( "Cannot find any device" );
		goto main_exit;
	}
	*/

	//screen normilised buffer to draw
	buf_len = sizeof(char)*w->w;
	buf = malloc( buf_len );

	sample_len = SAMPLE_LENGHT;
	sample_buf = malloc( sample_len );
	
	srand(0); //fake seed
	for ( i=0; i<400;i++ )
	{
		//for (j=0; j<buf_len; j++)
		//	sample_buf[j] = (uint8_t)((rand()&0xff));

		//read some samples
		dongle_read_samples( dongle, sample_buf, sample_len );

		//do fft
		simple_fft( sample_buf, sample_len );

		//prepare to show on the screen
		//if (normalise( sample_buf, sample_len, buf, buf_len ) == -1)
		{
			//printf("Cannot normalise\n");
		}
		glui_waterfall_data( t, sample_len/2, sample_buf );
		//printf("\n\b");
		usleep(10000);
	}

main_exit:
	//close gui, restore terminal mode
	glui_close( t );
	sdr_close( sdr );

	return 0;
}