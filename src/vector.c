#include <stdio.h>
#include <string.h>
#include <math.h>

#include "vector.h"

void vector_add(const float *a, const float *b, float *dst, const int size)
{
	for (int i = 0; i < size; i++)
		dst[i] = a[i] + b[i];
}

void vector_subtract(const float *a, const float *b, float *dst, const int size)
{
	for (int i = 0; i < size; i++)
		dst[i] = a[i] - b[i];
}

void vector_scale(float *x, const float s, const int size)
{
	for (int i = 0; i < size; i++)
		x[i] *= s;
}

float vector_dot(const float *a, const float *b, const int size)
{
	float dot = 0.0f;
	for (int i = 0; i < size; i++)
		dot += a[i] * b[i];

	return dot;
}

void vector3_cross(const float *a, const float *b, float *dst)
{
	dst[0] = (a[1] * b[2]) - (a[2] * b[1]);
	dst[1] = (a[2] * b[0]) - (a[0] * b[2]);
	dst[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

void vector_normalize(float *x, const int size)
{
	float mag = sqrtf(vector_dot(x, x, size));
	vector_scale(x, 1.0f / mag, size);
}
