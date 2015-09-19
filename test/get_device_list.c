#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//rtlsdr
#include <rtl-sdr.h>   

int main(  )
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
		rtlsdr_get_device_usb_strings( i, manuf, product, serial );
		printf("\\
     Manufact:%s\n\\
     Product:%s\n\\
     Serial:%s\n", manuf, product, serial );
	}

	return 0;
}