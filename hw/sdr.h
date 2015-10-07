#ifndef __RADIOLA_HW_SDR_H
#define __RADIOLA_HW_SDR_H

#include "hw.h"

typedef enum
{
	DEVICE_NONE=0,
	DEVICE_RTLSDR,
	DEVICE_AUDIO,
	DEVICE_NET,
	DEVICE_PTT
} sdr_device;


typedef struct dongle_t
{
	rtlsdr_dev_t *dev;
	int dev_index;
	uint32_t freq;
	uint32_t rate;
	int gain;
} dongle_t;


typedef struct audio_t
{
} audio_t;


typedef struct sdr_t 
{
	sdr_device dev_type;
	dongle_t   *dongle;
	uint32_t   d_used;
	audio_t    *audio;
	uint32_t   a_used;
} sdr_t;

//index of device in oter list
//return inner device list
int sdr_open_device( sdr_t *sdr, int dev_index );
//get index in list of devices of structure from outer device index
//? do we need?
//int sdr_get_dongle_idx( sdr_t *sdr, int idx );
//close device by internal list index
int sdr_close_device( sdr_t *sdr, int idx );

int dongle_stop( sdr_t *sdr, int idx );
//int dongle_open( sdr_t *);
int dongle_set_freq( sdr_t *sdr, int idx, uint32_t freq);
int dongle_set_gain( sdr_t *sdr, int idx, int gain);
int dongle_set_sample_rate( sdr_t *sdr, int idx, uint32_t srate );

uint32_t dongle_get_freq( sdr_t *sdr, int idx );
int dongle_get_gain( sdr_t *sdr, int idx);
uint32_t dongle_get_sample_rate( sdr_t *sdr, int idx );
int dongle_get_samples( sdr_t *sdr, int idx, uint8_t *buf, int len );
//int dongle_close();

#endif