#include <math.h>

#include "vector.h"

float vector_dot(const float *a, const float *b, const int size)
{
	float dot = 0.0f;

	for (int i = 0; i < size; i++)
		dot += a[i] * b[i];

	return (dot);
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
