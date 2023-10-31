#ifndef CAMERA_H_
#define CAMERA_H_

typedef struct
{
	float focus[3];
	float view_angle[2];
	float zoom;
} camera_t;

void camera_init(camera_t *c);
void camera_eye(const camera_t *c, float *out);
void camera_forw_side_up(const camera_t *c, float *eye,
			 float *forw, float *side, float *up);

#endif /* CAMERA_H_ */
