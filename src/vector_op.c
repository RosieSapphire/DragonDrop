#include "vector.h"

void vector_add(const float *a, const float *b, float *dst, const int size)
{
	for (int i = 0; i < size; i++)
		dst[i] = a[i] + b[i];
}

void vector_subtract(const float *a, const float *b,
		     float *dst, const int size)
{
	for (int i = 0; i < size; i++)
		dst[i] = a[i] - b[i];
}

void vector_scale(float *x, const float s, const int size)
{
	for (int i = 0; i < size; i++)
		x[i] *= s;
}
