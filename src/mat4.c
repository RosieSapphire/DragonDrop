#include "mat4.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

void mat4_identity(float m[4][4])
{
	memset(m, 0, sizeof(float) * 16);
	for (int i = 0; i < 4; i++)
		m[i][i] = 1;
}

void mat4_perspective(float m[4][4], float fov_deg, float aspect,
		      float near, float far)
{
	float fov_itan = 1.0f / tanf(fov_deg * DEG_TO_RAD * 0.5f);
	float z_range = 1.0f / (near - far);

	memset(m, 0, sizeof(float) * 16);
	m[0][0] = fov_itan / aspect;
	m[1][1] = fov_itan;
	m[2][2] = (near + far) * z_range;
	m[2][3] = -1.0f;
	m[3][2] = 2.0f * near * far * z_range;
}

void mat4_rotate(float m[4][4], float axis[3], float radang)
{
	const float sinang = sinf(radang);
	const float cosang = cosf(radang);
	float rot[4][4];

	memset(m, 0, sizeof(float) * 16);
	rot[0][0] = cosang + (axis[0] * axis[0]) * (1 - cosang);
	rot[1][0] = axis[0] * axis[1] * (1 - cosang) - axis[2] * sinang;
	rot[2][0] = axis[0] * axis[2] * (1 - cosang) + axis[1] * sinang;

	rot[0][1] = axis[1] * axis[0] * (1 - cosang) + axis[2] * sinang;
	rot[1][1] = cosang + (axis[1] * axis[1]) * (1 - cosang);
	rot[2][1] = axis[1] * axis[2] * (1 - cosang) - axis[0] * sinang;

	rot[0][2] = axis[2] * axis[0] * (1 - cosang) - axis[1] * sinang;
	rot[1][2] = axis[2] * axis[1] * (1 - cosang) + axis[0] * sinang;
	rot[2][2] = cosang + (axis[2] * axis[2]) * (1 - cosang);

	rot[3][3] = 1.0f;

	mat4_multiply(rot, m, m);
}

void mat4_multiply(float a[4][4], float b[4][4], float out[4][4])
{
	float tmp[4][4];

	memset(tmp, 0, sizeof(float) * 16);
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			for (int ind = 0; ind < 4; ind++)
				tmp[col][row] += a[ind][row] * b[col][ind];

	memcpy(out, tmp, sizeof(float) * 4 * 4);
}

void mat4_lookat(float m[4][4], float eye[3], float focus[3], float gup[3])
{
	float front[3], side[3], up[3];

	memset(m, 0, sizeof(float) * 16);
	vector_subtract(focus, eye, front, 3);
	vector_normalize(front, 3);
	vector3_cross(gup, front, side);
	vector_normalize(side, 3);
	vector3_cross(front, side, up);

	m[0][0] =   side[0];
	m[0][1] =     up[0];
	m[0][2] = -front[0];
	m[0][3] =  0.0f;

	m[1][0] =   side[1];
	m[1][1] =     up[1];
	m[1][2] = -front[1];
	m[1][3] =  0.0f;

	m[2][0] =   side[2];
	m[2][1] =     up[2];
	m[2][2] = -front[2];
	m[2][3] =  0.0f;

	m[3][0] = vector_dot(side,  eye, 3);
	m[3][1] = vector_dot(up,    eye, 3);
	m[3][2] = vector_dot(front, eye, 3);
	m[3][3] = 1.0f;
}
