#include "util.h"

float clampf(float x, float min, float max)
{
	if(x > max)
		return max;

	if(x < min)
		return min;

	return x;
}
