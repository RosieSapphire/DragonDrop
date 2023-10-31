#include <math.h>

#include "vector.h"
#include "camera.h"

float camera_focus[3] = {0, 0, 0};
float camera_view_angle[2] = {0, 0};
float camera_zoom = 3.0f;

void camera_eye(float *out)
{
	const float cosv1 = cosf(camera_view_angle[1]);
	vector_copy(camera_focus, out, 3);
	out[0] += cosf(camera_view_angle[0]) * cosv1 * camera_zoom;
	out[1] += sinf(camera_view_angle[0]) * cosv1 * camera_zoom;
	out[2] += sinf(camera_view_angle[1]) * camera_zoom;
}

void camera_forw_side_up(float *eye, float *forw, float *side, float *up)
{
	vector_subtract(camera_focus, eye, forw, 3);
	vector_normalize(forw, 3);
	float up1[3] = {0, 0, 1};
	vector3_cross(forw, up1, side);
	vector3_cross(forw, side, up);
	vector_negate(up, 3);
}
