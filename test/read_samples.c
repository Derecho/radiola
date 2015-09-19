#include <stdio.h>
#include <stdlib.h>

//r820t
#include <rtl-sdr.h>

void read_samples( uint32_t dev_index )
{
	const uint32_t SAMPLE_RATE = 2048000;
	const int DEFAULT_BUF_LENGTH = (16 * 16384);


	uint8_t *buffer=NULL;
	int ret, n_read;
	static rtlsdr_dev_t *dev = NULL;

	//open tunner
	ret = rtlsdr_open(&dev, (uint32_t)dev_index);
	if ( ret < 0 )
	{
		printf("Cannot open device %02d\n", dev_index );
		return;
	}

	ret = rtlsdr_set_sample_rate(dev, SAMPLE_RATE);
	if ( ret < 0 )
	{
		printf("Coulnt set sample rate to %d\n", SAMPLE_RATE);
		return;
	}

	ret = rtlsdr_reset_buffer(dev);
	if ( ret < 0 )
	{
		printf("Couldnt reset buffer\n");
		return;
	}

	buffer = malloc(DEFAULT_BUF_LENGTH * sizeof(uint8_t));
	ret = rtlsdr_read_sync( dev, buffer, DEFAULT_BUF_LENGTH, &n_read );
	if (ret < 0)
	{
		printf("sync read failed\n");
		return;
	}
	free( buffer );


	//close tunner
	rtlsdr_close( dev );

	printf("     SUCESFULL read\n");
}

int main()
{
	int i;
	uint32_t dev_num;

	dev_num = rtlsdr_get_device_count();
	printf("Found %d device(s)\n", dev_num );

	printf("List device names:\n");
	for (i=0; i<dev_num; i++)
	{
		char manuf[256], product[256], serial[256];
		const char *dn = rtlsdr_get_device_name( i );
		printf(" %02d. Device Name: %s\n", i, dn );
		read_samples( i );
	}

	return 0;
}