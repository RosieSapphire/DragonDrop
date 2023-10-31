#include <stdio.h>
#include <string.h>
#include <math.h>

#include "vector.h"

void vector_zero(float *x, const int size)
{
	memset(x, 0, sizeof(float) * size);
}

void vector_copy(float *dst, const float *src, const int size)
{
	memcpy(dst, src, sizeof(float) * size);
}

void vector_printf(const float *x, const int size)
{
	for (int i = 0; i < size; i++)
		printf("%f ", x[i]);

	printf("\n");
}
