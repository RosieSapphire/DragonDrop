#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

float clampf(float x, float min, float max);
uint16_t uint16_endian_flip(uint16_t x);
uint32_t uint32_endian_flip(uint32_t x);
float float_endian_flip(float x);

#endif /* UTIL_H_ */
