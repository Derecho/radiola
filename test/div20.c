#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/*
Page about div
http://bisqwit.iki.fi/story/howto/bitmath/#DivAndModDivisionAndModulo
*/

uint8_t div20u8( uint16_t num )
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

int main()
{
	uint32_t a = 10000;
	uint32_t b = 5;
	uint32_t r = 0;
	uint32_t m = 1;
	printf("%04x(%d) %04x(%3d) %04x(%3d) %04x(%3d)\n",a,a,b,b,r,r,m,m);
	while ( b < a )
	{
		b = b << 1;
		m = m << 1;
		printf("%04x(%3d) %04x(%3d) %04x(%3d) %04x(%3d)\n",a,a,b,b,r,r,m,m);
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
		printf("%04x(%3d) %04x(%3d) %04x(%3d) %04x(%3d)\n",a,a,b,b,r,r,m,m);
	} while ( m != 0 );
	return 0;
}