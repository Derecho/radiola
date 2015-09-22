#include "hw.h"

uint32_t hw_get_device_count(void)
{
	uint32_t ret;
	ret = rtlsdr_get_device_count();
	return ret;
}


const char* hw_get_device_name(uint32_t index)
{
	const char *ret;
	ret = rtlsdr_get_device_name( index );
	return ret;
}


int hw_get_device_usb_strings(uint32_t index,
					     char *manufact,
					     char *product,
					     char *serial)
{
	int ret;
	ret = rtlsdr_get_device_usb_strings( index, manufact, product, serial );
	return ret;
}


int hw_get_index_by_serial(const char *serial)
{
	int ret;
	rtlsdr_get_index_by_serial( serial );
	return ret;
}


int hw_open(rtlsdr_dev_t **dev, uint32_t index)
{
	int ret;
	ret = rtlsdr_open( dev, index );
	return ret;
}


int hw_close(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_close( dev );
	return ret;
}


int hw_set_xtal_freq(rtlsdr_dev_t *dev, uint32_t rtl_freq,
				    uint32_t tuner_freq)
{
	int ret;
	ret = rtlsdr_set_xtal_freq( dev, rtl_freq, tuner_freq );
	return ret;
}


int hw_get_xtal_freq(rtlsdr_dev_t *dev, uint32_t *rtl_freq,
				    uint32_t *tuner_freq)
{
	int ret;
	ret = rtlsdr_get_xtal_freq( dev, rtl_freq, tuner_freq );
	return ret;
}


int hw_get_usb_strings(rtlsdr_dev_t *dev, char *manufact,
				      char *product, char *serial)
{
	int ret;
	ret = rtlsdr_get_usb_strings( dev, manufact, product, serial );
	return ret;
}


int hw_write_eeprom(rtlsdr_dev_t *dev, uint8_t *data,
				  uint8_t offset, uint16_t len)
{
	int ret;
	ret = rtlsdr_write_eeprom( dev, data, offset, len );
	return ret;
}


int hw_read_eeprom(rtlsdr_dev_t *dev, uint8_t *data,
				  uint8_t offset, uint16_t len)
{
	int ret;
	ret = rtlsdr_read_eeprom( dev, data, offset, len );
	return ret;
}


int hw_set_center_freq(rtlsdr_dev_t *dev, uint32_t freq)
{
	int ret;
	ret = rtlsdr_set_center_freq( dev, freq );
	return ret;
}


uint32_t hw_get_center_freq(rtlsdr_dev_t *dev)
{
	uint32_t ret;
	ret = rtlsdr_get_center_freq( dev );
	return ret;
}	


int hw_set_freq_correction(rtlsdr_dev_t *dev, int ppm)
{
	int ret;
	ret = rtlsdr_set_freq_correction( dev, ppm );
	return ret;
}


int hw_get_freq_correction(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_get_freq_correction( dev );
	return ret;
}


enum rtlsdr_tuner hw_get_tuner_type(rtlsdr_dev_t *dev)
{
	enum rtlsdr_tuner ret;
	ret = rtlsdr_get_tuner_type( dev );
	return ret;
}


int hw_get_tuner_gains(rtlsdr_dev_t *dev, int *gains)
{
	int ret;
	ret = rtlsdr_get_tuner_gains( dev, gains );
	return ret;
}


int hw_set_tuner_gain(rtlsdr_dev_t *dev, int gain)
{
	int ret;
	ret = rtlsdr_set_tuner_gain( dev, gain );
	return ret;
}


int hw_get_tuner_gain(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_get_tuner_gain( dev );
	return ret;
}


int hw_set_tuner_if_gain(rtlsdr_dev_t *dev, int stage, int gain)
{
	int ret;
	ret = rtlsdr_set_tuner_if_gain( dev, stage, gain );
	return ret;
}


int hw_set_tuner_gain_mode(rtlsdr_dev_t *dev, int manual)
{
	int ret;
	ret = rtlsdr_set_tuner_gain_mode( dev, manual );
	return ret;
}


int hw_set_sample_rate(rtlsdr_dev_t *dev, uint32_t rate)
{
	int ret;
	ret = rtlsdr_set_sample_rate( dev, rate );
	return ret;
}


uint32_t hw_get_sample_rate(rtlsdr_dev_t *dev)
{
	uint32_t ret;
	ret = rtlsdr_get_sample_rate( dev );
	return ret;
}


int hw_set_testmode(rtlsdr_dev_t *dev, int on)
{
	int ret;
	ret = rtlsdr_set_testmode( dev, on );
	return ret;
}


int hw_set_agc_mode(rtlsdr_dev_t *dev, int on)
{
	int ret;
	ret = rtlsdr_set_agc_mode( dev, on );
	return ret;
}


int hw_set_direct_sampling(rtlsdr_dev_t *dev, int on)
{
	int ret;
	ret = rtlsdr_set_direct_sampling( dev, on );
	return ret;
}


int hw_get_direct_sampling(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_get_direct_sampling( dev );
	return ret;
}


int hw_set_offset_tuning(rtlsdr_dev_t *dev, int on)
{
	int ret;
	ret = rtlsdr_set_offset_tuning( dev, on );
	return ret;
}


int hw_get_offset_tuning(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_get_offset_tuning( dev );
	return ret;
}



int hw_reset_buffer(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_reset_buffer( dev );
	return ret;
}


int hw_read_sync(rtlsdr_dev_t *dev, void *buf, int len, int *n_read)
{
	int ret;
	ret = rtlsdr_read_sync( dev, buf, len, n_read );
	return ret;
}


int hw_wait_async(rtlsdr_dev_t *dev, rtlsdr_read_async_cb_t cb, void *ctx)
{
	int ret;
	ret = rtlsdr_wait_async( dev, cb, ctx );
	return ret;
}


int hw_read_async(rtlsdr_dev_t *dev,
				 rtlsdr_read_async_cb_t cb,
				 void *ctx,
				 uint32_t buf_num,
				 uint32_t buf_len)
{
	int ret;
	ret = rtlsdr_read_async( dev, cb, ctx, buf_num, buf_len );
	return ret;
}


int hw_cancel_async(rtlsdr_dev_t *dev)
{
	int ret;
	ret = rtlsdr_cancel_async( dev );
	return ret;
}