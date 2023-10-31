#ifndef CAMERA_H_
#define CAMERA_H_

extern float camera_focus[3];
extern float camera_view_angle[2];
extern float camera_zoom;

void camera_eye(float *out);
void camera_forw_side_up(float *eye, float *forw, float *side, float *up);

#endif /* CAMERA_H_ */
