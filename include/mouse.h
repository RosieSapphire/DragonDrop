#ifndef MOUSE_H_
#define MOUSE_H_

#include <GLFW/glfw3.h>

#include "object.h"
#include "camera.h"

enum input_mode
{
	IMODE_NORMAL,
	IMODE_MOVE,
	IMODE_ROTATE,
	IMODE_SCALE,
	IMODE_NUM_STATES,
};

typedef struct
{
	enum input_mode input_mode;
	double last_pos[2];
	int axis_move;
} mouse_t;

/* general */
void mouse_init(mouse_t *m);
void mouse_get_delta(mouse_t *m, GLFWwindow *win, double *mnow, double *md);
void mouse_input(mouse_t *m, camera_t *c, GLFWwindow *window, object_t *obj_cur);

/* camera */
void mouse_camera_orbiting(mouse_t *m, camera_t *c, GLFWwindow *window);
void mouse_camera_zooming(mouse_t *m, camera_t *c, GLFWwindow *window);
void mouse_camera_panning(mouse_t *m, camera_t *c, GLFWwindow *window);

/* object */
void mouse_object_moving(mouse_t *m, camera_t *c,
			 GLFWwindow *window, object_t *obj_cur);

#endif /* MOUSE_H_ */
