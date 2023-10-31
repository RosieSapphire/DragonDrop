#ifndef VECTOR_H_
#define VECTOR_H_

/* general */
void vector_zero(float *x, const int size);
void vector_copy(float *dst, const float *src, const int size);
void vector_printf(const float *x, const int size);

/* ops */
void vector_add(const float *a, const float *b, float *dst, const int size);
void vector_subtract(const float *a, const float *b,
		     float *dst, const int size);
void vector_scale(float *x, const float s, const int size);

/* trig */
float vector_dot(const float *a, const float *b, const int size);
void vector3_cross(const float *a, const float *b, float *dst);
void vector_normalize(float *x, const int size);

#endif /* VECTOR_H_ */
