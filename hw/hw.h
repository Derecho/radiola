#ifndef __RADIOLA_HW_H
#define __RADIOLA_HW_H

#include <stdio.h>
#include <stdlib.h>

//#include <r820t.h>
#include <rtl-sdr.h>

uint32_t hw_get_device_count(void);
const char* hw_get_device_name(uint32_t index);
int hw_get_device_usb_strings(uint32_t index,
					     char *manufact,
					     char *product,
					     char *serial);
int hw_get_index_by_serial(const char *serial);
int hw_open(rtlsdr_dev_t **dev, uint32_t index);
int hw_close(rtlsdr_dev_t *dev);
int hw_set_xtal_freq(rtlsdr_dev_t *dev, uint32_t rtl_freq,
				    uint32_t tuner_freq);
int hw_get_xtal_freq(rtlsdr_dev_t *dev, uint32_t *rtl_freq,
				    uint32_t *tuner_freq);
int hw_get_usb_strings(rtlsdr_dev_t *dev, char *manufact,
				      char *product, char *serial);				 			    
int hw_write_eeprom(rtlsdr_dev_t *dev, uint8_t *data,
				  uint8_t offset, uint16_t len);
int hw_read_eeprom(rtlsdr_dev_t *dev, uint8_t *data,
				  uint8_t offset, uint16_t len);
int hw_set_center_freq(rtlsdr_dev_t *dev, uint32_t freq);
uint32_t hw_get_center_freq(rtlsdr_dev_t *dev);
int hw_set_freq_correction(rtlsdr_dev_t *dev, int ppm);
int hw_get_freq_correction(rtlsdr_dev_t *dev);
enum rtlsdr_tuner hw_get_tuner_type(rtlsdr_dev_t *dev);
int hw_get_tuner_gains(rtlsdr_dev_t *dev, int *gains);
int hw_set_tuner_gain(rtlsdr_dev_t *dev, int gain);
int hw_get_tuner_gain(rtlsdr_dev_t *dev);
int hw_set_tuner_if_gain(rtlsdr_dev_t *dev, int stage, int gain);
int hw_set_tuner_gain_mode(rtlsdr_dev_t *dev, int manual);
int hw_set_sample_rate(rtlsdr_dev_t *dev, uint32_t rate);
uint32_t hw_get_sample_rate(rtlsdr_dev_t *dev);
int hw_set_testmode(rtlsdr_dev_t *dev, int on);
int hw_set_agc_mode(rtlsdr_dev_t *dev, int on);
int hw_set_direct_sampling(rtlsdr_dev_t *dev, int on);
int hw_get_direct_sampling(rtlsdr_dev_t *dev);
int hw_set_offset_tuning(rtlsdr_dev_t *dev, int on);
int hw_get_offset_tuning(rtlsdr_dev_t *dev);
int hw_reset_buffer(rtlsdr_dev_t *dev);
int hw_read_sync(rtlsdr_dev_t *dev, void *buf, int len, int *n_read);
int hw_wait_async(rtlsdr_dev_t *dev, rtlsdr_read_async_cb_t cb, void *ctx);
int hw_read_async(rtlsdr_dev_t *dev,
				 rtlsdr_read_async_cb_t cb,
				 void *ctx,
				 uint32_t buf_num,
				 uint32_t buf_len);
int hw_cancel_async(rtlsdr_dev_t *dev);






#endif