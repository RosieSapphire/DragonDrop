#include <math.h>

#include "util.h"
#include "config.h"
#include "vector.h"
#include "camera.h"
#include "object.h"
#include "mouse.h"

static enum input_mode input_mode = IMODE_NORMAL;
static double mouse_last[2] = {0, 0};
static int axis_move = -1;

void mouse_input_orbiting(GLFWwindow *window)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
		return;

	double mouse_now[2];
	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);
	double mouse_delta[2] = {
		mouse_now[0] - mouse_last[0],
		mouse_now[1] - mouse_last[1],
	};
	mouse_last[0] = mouse_now[0];
	mouse_last[1] = mouse_now[1];
	camera_view_angle[0] += mouse_delta[0] * 0.02f;
	camera_view_angle[1] += mouse_delta[1] * 0.02f;
	camera_view_angle[1] = clampf(camera_view_angle[1], -1.5f, 1.5f);
}

void mouse_input_zooming(GLFWwindow *window)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
		return;

	double mouse_y_now;
	glfwGetCursorPos(window, NULL, &mouse_y_now);
	double mouse_y_delta = mouse_y_now - mouse_last[1];
	mouse_last[1] = mouse_y_now;
	camera_zoom += mouse_y_delta * 0.01f;
	camera_zoom = fmaxf(camera_zoom, 0);
}

void mouse_input_panning(GLFWwindow *window)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		return;
	}

	double mouse_now[2];
	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);

	if (mouse_now[0] <= 420 || mouse_now[0] >= CONF_WIDTH - 240) {
		return;
	}

	double mouse_delta[2] = {
		mouse_now[0] - mouse_last[0],
		mouse_now[1] - mouse_last[1],
	};

	float eye[3] = {0, 0, 0};
	camera_eye(eye);
	float forw[3], side[3], up[3];
	camera_forw_side_up(eye, forw, side, up);
	float move[3];
	vector_scale(up, mouse_delta[1] * 0.02f, 3);
	vector_scale(side, mouse_delta[0] * 0.02f, 3);
	vector_add(up, side, move, 3);

	vector_add(camera_focus, move, camera_focus, 3);
}

void mouse_input_moving(GLFWwindow *window)
{
	static int axis_key_last[3] = {0, 0, 0};
	int axis_key_now[3] = {
		glfwGetKey(window, GLFW_KEY_X),
		glfwGetKey(window, GLFW_KEY_Y),
		glfwGetKey(window, GLFW_KEY_Z),
	};

	for (int i = 0; i < 3; i++) {
		if (!axis_key_last[i] && axis_key_now[i]) {
			if (i == axis_move) {
				axis_move = -1;
			} else {
				axis_move = i;
			}
		}
		axis_key_last[i] = axis_key_now[i];
	}

	double mouse_now[2];
	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);
	double mouse_delta[2] = {
		mouse_now[0] - mouse_last[0],
		mouse_now[1] - mouse_last[1],
	};
	mouse_last[0] = mouse_now[0];
	mouse_last[1] = mouse_now[1];

	float *obj_pos = object_selected->trans[3];

	float cam_eye[3], forw[3], side[3], up[3];
	camera_eye(cam_eye);
	camera_forw_side_up(cam_eye, forw, side, up);
	vector_scale(up, -mouse_delta[1] * 0.02f, 3);
	vector_scale(side, -mouse_delta[0] * 0.02f, 3);
	float move[3];
	vector_add(up, side, move, 3);

	switch(axis_move) {
	case -1:
		break;

	case 0:
		move[0] *= 1;
		move[1] *= 0;
		move[2] *= 0;
		break;
	
	case 1:                             
		move[0] *= 0;
		move[1] *= 1;
		move[2] *= 0;
		break;

	case 2:                             
		move[0] *= 0;
		move[1] *= 0;
		move[2] *= 1;
		break;
	}

	vector_add(obj_pos, move, obj_pos, 3);
}

void mouse_input(GLFWwindow *window)
{
	static int key_g_last = 0;
	static int right_click_last = 0;
	int key_g_now = glfwGetKey(window, GLFW_KEY_G);
	int right_click_now =
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	static float last_normal_pos[3] = {0, 0, 0};
	switch(input_mode) {
	case IMODE_NORMAL:
		mouse_input_orbiting(window);
		mouse_input_zooming(window);
		mouse_input_panning(window);
		if (key_g_now && !key_g_last && object_selected) {
			vector_copy(object_selected->trans[3],
				last_normal_pos, 3);
			input_mode = IMODE_MOVE;
		}
		break;

	case IMODE_MOVE:
		mouse_input_moving(window);
		if ((key_g_now && !key_g_last) ||
				glfwGetMouseButton(window,
			GLFW_MOUSE_BUTTON_LEFT)) {
			axis_move = -1;
			input_mode = IMODE_NORMAL;
		}

		if ((right_click_now && !right_click_last) ||
				glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			axis_move = -1;
			input_mode = IMODE_NORMAL;
			vector_copy(last_normal_pos,
	       				object_selected->trans[3], 3);
		}
		break;

	default:
		break;
	}
	key_g_last = key_g_now;
	right_click_last = right_click_now;

	glfwGetCursorPos(window, mouse_last + 0, mouse_last + 1);
}
