#include <math.h>
#include <string.h>

#include "vector.h"
#include "camera.h"

void camera_init(camera_t *c)
{
	memset(c, 0, sizeof(camera_t));
	c->zoom = 3.0f;
}

void camera_eye(const camera_t *c, float *out)
{
	const float cosv1 = cosf(c->view_angle[1]);

	memcpy(out, c->focus, sizeof(float) * 3);
	out[0] += cosf(c->view_angle[0]) * cosv1 * c->zoom;
	out[1] += sinf(c->view_angle[0]) * cosv1 * c->zoom;
	out[2] += sinf(c->view_angle[1]) * c->zoom;
}

void camera_forw_side_up(const camera_t *c, float *eye,
			 float *forw, float *side, float *up)
{
	float up1[3] = {0, 0, 1};

	vector_subtract(c->focus, eye, forw, 3);
	vector_normalize(forw, 3);
	vector3_cross(forw, up1, side);
	vector3_cross(forw, side, up);
	vector_scale(up, -1.0f, 3);
}
