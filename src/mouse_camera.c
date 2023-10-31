#include <math.h>

#include "vector.h"
#include "config.h"
#include "util.h"
#include "mouse.h"

void mouse_camera_orbiting(mouse_t *m, camera_t *c, GLFWwindow *window)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
		return;

	double mouse_now[2];
	double mouse_delta[2];

	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);
	mouse_get_delta(m, window, mouse_now, mouse_delta);

	c->view_angle[0] += mouse_delta[0] * 0.02f;
	c->view_angle[1] += mouse_delta[1] * 0.02f;
	c->view_angle[1] = clampf(c->view_angle[1], -1.5f, 1.5f);
}

void mouse_camera_zooming(mouse_t *m, camera_t *c, GLFWwindow *window)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
		return;

	double mouse_y_now;

	glfwGetCursorPos(window, NULL, &mouse_y_now);
	double mouse_y_delta = mouse_y_now - m->last_pos[1];

	m->last_pos[1] = mouse_y_now;
	c->zoom += mouse_y_delta * 0.01f;
	c->zoom = fmaxf(c->zoom, 0);
}

void mouse_camera_panning(mouse_t *m, camera_t *c, GLFWwindow *window)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		return;

	double mouse_now[2];
	double mouse_delta[2];

	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);
	if (mouse_now[0] <= 420 || mouse_now[0] >= CONF_WIDTH - 240)
		return;

	mouse_get_delta(m, window, mouse_now, mouse_delta);

	float eye[3] = {0, 0, 0}, forw[3], side[3], up[3], move[3];

	camera_eye(c, eye);
	camera_forw_side_up(c, eye, forw, side, up);
	vector_scale(up, mouse_delta[1] * 0.02f, 3);
	vector_scale(side, mouse_delta[0] * 0.02f, 3);
	vector_add(up, side, move, 3);
	vector_add(c->focus, move, c->focus, 3);
}
