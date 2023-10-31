#ifndef MOUSE_H_
#define MOUSE_H_

#include <GLFW/glfw3.h>

enum input_mode
{
	IMODE_NORMAL,
	IMODE_MOVE,
	IMODE_ROTATE,
	IMODE_SCALE,
	IMODE_NUM_STATES,
};

void mouse_input_orbiting(GLFWwindow *window);
void mouse_input_zooming(GLFWwindow *window);
void mouse_input_panning(GLFWwindow *window);
void mouse_input_moving(GLFWwindow *window);
void mouse_input(GLFWwindow *window);

#endif /* MOUSE_H_ */
