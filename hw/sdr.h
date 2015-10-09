#ifndef __RADIOLA_HW_SDR_H
#define __RADIOLA_HW_SDR_H

#include <string.h>

#include "hw.h"


//list of supported devices RTLSDR is rtlsdr tunners,
//AUDIO is just used audio cards
//NET get or send some info to server/client
//PTT usb connected with audio link radios, trought PTT things
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

#define DONGLE_SAMPLE_RATE 2048000
#define DONGLE_CENTER_FREQ 100000000
#define DONGLE_GAIN        1

//dongle pointer to dongle_t
#define DEF_DONGLE_VALUES(DONGLE)\
{\
(DONGLE)->freq=DONGLE_CENTER_FREQ;\
(DONGLE)->rate=DONGLE_SAMPLE_RATE;\
(DONGLE)->gain=DONGLE_GAIN;\
}


typedef struct audio_t
{
} audio_t;


typedef struct sdr_t 
{
	dongle_t   *dongle; //list of rtlsdr devices
	uint32_t   d_used;  //not yes used
	audio_t    *audio;  //audio devices used
	uint32_t   a_used;  //not yet used
} sdr_t;

//init structure 

sdr_t* sdr_init(); 
//index of device in oter list
//return inner device list
int sdr_open_device( sdr_t *sdr, int dev_index );
//associate audio device that going to be used
//int sdr_open_audio( sdr_t *sdr, int dev_index );//?should be just device from list or device name?


//get device structure from sdr manager
dongle_t* sdr_get_device_id( sdr_t *sdr, int dev_index);

//get index in list of devices of structure from outer device index
//? do we need?
//int sdr_get_dongle_idx( sdr_t *sdr, int idx );
//close device by internal list index
int sdr_close_device( sdr_t *sdr, int idx );

//close sdr
//if there is opened audios then close
//if there is opened devices then close
int sdr_close( sdr_t *sdr );

//stop any action that associated with dongle
int dongle_stop( dongle_t *dongle );
//int dongle_open( sdr_t *);
//set dongle frequency
int dongle_set_freq( dongle_t *dongle, uint32_t freq);
//set gain
int dongle_set_gain( dongle_t *dongle, int gain);
int dongle_set_agc( dongle_t *dongle, int mode);
//set dongle sample rate
int dongle_set_sample_rate( dongle_t *dongle, uint32_t rate );

uint32_t dongle_get_freq( dongle_t *dongle );
int      dongle_get_gain( dongle_t *dongle );
uint32_t dongle_get_sample_rate( dongle_t *dongle );
int      dongle_read_samples( dongle_t *dongle, uint8_t *buf, int len );
//int dongle_close();

#endif