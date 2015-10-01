#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//radiola
#include <draw/tui.h>
#include <hw/hw.h>

#define SAMPLE_RATE  2048000

#define CENTER_FREQ  446500000
#define FFT_LEVEL    10
#define FFT_SIZE     (1 << FFT_LEVEL)
#define BUF_LENGHT   (2 * FFT_SIZE)
#define PRESCALE     8
#define POSTSCALE    2

static rtlsdr_dev_t *dev = NULL;

int16_t* Sinewave;
int N_WAVE, LOG2_N_WAVE;
double* power_table;

int sdr_init()
{
	int ret;
	uint32_t dev_index = 0;

	//open tunner
	ret = rtlsdr_open(&dev, (uint32_t)dev_index);
	if ( ret < 0 )
	{
		printf("Cannot open device %02d\n", dev_index );
		return -1;
	}

	ret = rtlsdr_reset_buffer(dev);
	if ( ret < 0 )
	{
		printf("Couldnt reset buffer\n");
		return -1;
	}

	ret = rtlsdr_set_sample_rate(dev, SAMPLE_RATE);
	if ( ret < 0 )
	{
		printf("Coulnt set sample rate to %d\n", SAMPLE_RATE);
		return -1;
	}

	ret = rtlsdr_set_center_freq( dev, CENTER_FREQ );
	if ( ret < 0 )
	{
		printf("Couldnt set freq %d\n", CENTER_FREQ);
		return -1;
	}

	ret = rtlsdr_set_tuner_gain_mode( dev, 1 );
	if ( ret < 0 )
	{
		printf("Cannot set auto gain mode\n");
		return -1;
	}

	ret = rtlsdr_set_agc_mode( dev, 1);
	if ( ret < 0 )
	{
		printf("Cannot set agc mode\n");
		return -1;
	}

	return 0;
}


int sdr_close()
{
	//close tunner
	if ( dev != NULL)
	{
		rtlsdr_close( dev );
		dev = NULL;
		return 0;
	}
	return -1;
}

int sdr_get_samples( uint8_t *buf, int len )
{
	int ret, read_num;

	ret = rtlsdr_read_sync( dev, buf, len, &read_num );
	if (ret < 0)
	{
		printf("sync read failed\n");
		return -1;
	}

	return 0;
}

//better to have size  size mod olen == 0
int normalise( uint8_t *ibuf, int ilen, uint8_t *obuf, int olen )
{
	int i,j,m;
	int ppi;

	if ( ilen >= olen )
	{
		ppi = ilen / olen;
	} else {
		return -1;
	}
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
	int fft_len;

	for (i=0; i<len; i++)
	{
		buf[i] = buf[i] * PRESCALE;
	}

	fix_fft( (uint16_t *)buf, FFT_LEVEL );

	for (i=0; i<FFT_SIZE; i++)
	{
		//buf1[i] = rtl_out.buf[i];
		//p = buf1[i] * buf1[i];
		j = i*2;
		p = (int16_t)real_conj(buf[j], buf[j + 1]);
		buf[i] += p;
	}

	fft_len = FFT_SIZE / 2;
	for (i=0; i<fft_len; i++)
	{
		buf[i] =         (int)log10(POSTSCALE * (float)buf[i+fft_len]);
		buf[i+fft_len] = (int)log10(POSTSCALE * (float)buf[i]);
	}

	return 0;
}

int main()
{



	return 0;
}