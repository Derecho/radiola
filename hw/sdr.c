#include "sdr.h"

sdr_t* sdr_init()
{
	sdr_t *sdr=NULL;

	sdr = malloc( sizeof(sdr_t) );
	if (sdr == NULL)
		return NULL;

	memset( sdr, 0, sizeof(sdr_t) );

	return sdr;
}


//index of device in oter list
//return inner device list
int sdr_open_device( sdr_t *sdr, int dev_index )
{
	int ret;
	dongle_t *dng=NULL;
	rtlsdr_dev_t *rtl=NULL;

	//for now only one device will be supported
	if (sdr == NULL)
		return -1;

	if (sdr->dongle != NULL)
		return -1;

	dng = malloc(sizeof(dongle_t));
	memset(dng, 0, sizeof(dongle_t));
	//set default values for dongle_t
	DEF_DONGLE_VALUES(dng);

	ret = hw_open( &rtl, (uint32_t)dev_index );
	if ( ret < 0 )
	{
		printf("Cannot open device %02d\n", dev_index );
		goto exit_dng_err;
	}

	ret = hw_reset_buffer( rtl );
	if ( ret < 0 )
	{
		printf("Couldnt reset buffer\n");
		goto exit_dng_err;
	}

	ret = hw_set_sample_rate( rtl, dng->rate );
	if ( ret < 0 )
	{
		printf("Couldnt set sample rate to %d\n", dng->rate);
		goto exit_dng_err;
	}

	ret = hw_set_center_freq( rtl, dng->freq );
	if ( ret < 0 )
	{
		printf("Couldnt set frequency %d\n", dng->freq );
		goto exit_dng_err;
	}

	ret = hw_set_tuner_gain_mode( rtl, dng->gain );
	if ( ret < 0 )
	{
		printf("Cannot set autogain mode\n");
		goto exit_dng_err;
	}

	ret = hw_set_agc_mode( rtl, 1 );
	if ( ret < 0 )
	{
		printf("Cannot set agc mode\n");
		goto exit_dng_err;
	}

	dng->dev_index = dev_index;
	dng->dev = rtl;
	sdr->dongle = dng;

	return 0;

exit_dng_err:
	free( dng );

	return -1;
}


//associate audio device that going to be used
//int sdr_open_audio( sdr_t *sdr, int dev_index );//?should be just device from list or device name?


//get device structure from sdr manager by pluged device num
//dont try to free it muahaha
dongle_t* sdr_get_device_id( sdr_t *sdr, int dev_index)
{
	dongle_t *dng=NULL;

	if ( sdr == NULL )
		return NULL;

	if (sdr->dongle == NULL)
		return NULL;

	if (sdr->dongle->dev_index != dev_index)
	{
		printf("Cannot find device with such device ID\n");
		return NULL;
	}

	return sdr->dongle;
}



//get index in list of devices of structure from outer device index
//? do we need?
//int sdr_get_dongle_idx( sdr_t *sdr, int idx );
//close device by internal list index
int sdr_close_device( sdr_t *sdr, int dev_index )
{
	if ( sdr == NULL )
		return -1;
	
	if ( sdr->dongle == NULL )
	{
		return -1;
	}

	if ( sdr->dongle->dev == NULL )
		return -1;

	if ( sdr->dongle->dev_index != dev_index )
		return -1;

	hw_close( sdr->dongle->dev );
	free( sdr->dongle->dev );
	sdr->dongle->dev = NULL;

	free( sdr->dongle );
	sdr->dongle = NULL;

	return 0;
}



//close sdr
//if there is opened audios then close
//if there is opened devices then close
int sdr_close( sdr_t *sdr )
{
	if ( sdr != NULL )
	{	
		if ( sdr->dongle != NULL )
		{
			if ( sdr->dongle->dev == NULL )
			{
				hw_close( sdr->dongle->dev );
				free( sdr->dongle->dev );
				sdr->dongle->dev = NULL;
			}
			free( sdr->dongle );
			sdr->dongle = NULL;
		}
		free(sdr);
		sdr = NULL;
		return 0;
	}
	return -1;
}



//stop any action that associated with dongle
//not yet implemented
int dongle_stop( dongle_t *dongle )
{
	return -1;
}


//int dongle_open( sdr_t *);
//set dongle frequency
int dongle_set_freq( dongle_t *dongle, uint32_t freq)
{
	int ret = 0;
	if ( dongle == NULL )
		return -1;

	if ( dongle->dev == NULL )
		return -1;

	ret = hw_set_center_freq( dongle->dev, freq );
	if ( ret < 0 )
	{
		printf("Cannot set device frequency to %ud\n", freq);
		return -1;
	}

	return 0;
}


//set gain
int dongle_set_gain( dongle_t *dongle, int gain)
{

	int ret;

	if ( dongle == NULL )
		return -1;

	if ( dongle->dev == NULL )
		return -1;

	ret = hw_set_tuner_gain_mode( dongle->dev, gain );
	if ( ret < 0 )
	{
		printf("Cannot set tunner gain level %d\n", gain );
		return -1;
	}

	return 0;
}


//set dongle sample rate
int dongle_set_sample_rate( dongle_t *dongle, uint32_t rate )
{
	int ret;

	if ( dongle == NULL )
	{
		return -1;
	}

	if ( dongle->dev == NULL )
		return -1;

	ret = hw_set_sample_rate( dongle->dev, rate );
	if ( ret < 0 )
	{
		printf("Cannot set sample rate %ud\n", rate);
		return -1;
	}

	return 0;
}

int dongle_set_agc( dongle_t *dongle, int mode)
{
	int ret;

	if ( dongle == NULL )
		return -1;

	if ( dongle->dev == NULL )
		return -1;

	//need mode check things
	ret = hw_set_agc_mode( dongle->dev, mode );
	if ( ret < 0 )
	{
		printf("Cannot set agc gain mode\n");
		return -1;
	}

	return 0;
}


uint32_t dongle_get_freq( dongle_t *dongle )
{
	return dongle->freq;
}


int dongle_get_gain( dongle_t *dongle )
{
	return dongle->gain;
}


uint32_t dongle_get_sample_rate( dongle_t *dongle )
{
	return dongle->rate;
}


int dongle_read_samples( dongle_t *dongle, uint8_t *buf, int len )
{
	int ret, read_num;

	if ( dongle == NULL )
		return -1;

	if ( dongle->dev == NULL )
		return -1;



	ret = hw_read_sync( dongle->dev, buf, len, &read_num );
	if ( ret < 0 )
	{
		printf("Couldnt read samples\n");
		return -1;
	}

	return 0;
}