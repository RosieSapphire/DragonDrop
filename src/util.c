#include "util.h"

float clampf(float x, float min, float max)
{
	if(x > max)
		return max;

	if(x < min)
		return min;

	return x;
}

uint16_t uint16_endian_flip(uint16_t x)
{
	return (x >> 8) | (x << 8);
}

uint32_t uint32_endian_flip(uint32_t x)
{
	return ((x>>24)&0x000000ff) | ((x<< 8)&0x00ff0000) |
	       ((x>> 8)&0x0000ff00) | ((x<<24)&0xff000000);
}

float float_endian_flip(float x)
{
	uint32_t tmp = *((uint32_t *)&x);
	tmp = ((tmp << 8) & 0xFF00FF00) | ((tmp >> 8) & 0x00FF00FF); 
    	tmp = (tmp << 16) | (tmp >> 16);
	return *((float *)&tmp);
}
