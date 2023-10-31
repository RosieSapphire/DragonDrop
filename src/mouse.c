#include <math.h>
#include <string.h>

#include "util.h"
#include "config.h"
#include "vector.h"
#include "object.h"
#include "mouse.h"

static float last_normal_pos[3] = {0, 0, 0};
static int key_g_last;

void mouse_init(mouse_t *m)
{
	m->input_mode = IMODE_NORMAL;
	m->last_pos[0] = 0;
	m->last_pos[1] = 0;
	m->axis_move = -1;
}

void mouse_get_delta(mouse_t *m, GLFWwindow *win, double *mnow, double *md)
{
	glfwGetCursorPos(win, mnow + 0, mnow + 1);
	md[0] = mnow[0] - m->last_pos[0];
	md[1] = mnow[1] - m->last_pos[1];
	m->last_pos[0] = mnow[0];
	m->last_pos[1] = mnow[1];
}

static void _mouse_input_normal(mouse_t *m, camera_t *c,
				GLFWwindow *window,
				const bool key_g_now, object_t *obj_cur)
{
	mouse_camera_orbiting(m, c, window);
	mouse_camera_zooming(m, c, window);
	mouse_camera_panning(m, c, window);
	if (key_g_now && !key_g_last && obj_cur)
	{
		memcpy(last_normal_pos, obj_cur->trans[3],
			sizeof(float) * 3);
		m->input_mode = IMODE_MOVE;
	}
}

void mouse_input(mouse_t *m, camera_t *c, GLFWwindow *window, object_t *obj_cur)
{
	static int right_click_last;
	int key_g_now = glfwGetKey(window, GLFW_KEY_G);
	int right_click_now =
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	switch (m->input_mode)
	{
	case IMODE_NORMAL:
		_mouse_input_normal(m, c, window, key_g_now, obj_cur);
		break;

	case IMODE_MOVE:
		mouse_object_moving(m, c, window, obj_cur);
		if ((key_g_now && !key_g_last) ||
				glfwGetMouseButton(window,
			GLFW_MOUSE_BUTTON_LEFT))
		{
			m->axis_move = -1;
			m->input_mode = IMODE_NORMAL;
		}

		if ((right_click_now && !right_click_last) ||
				glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			m->axis_move = -1;
			m->input_mode = IMODE_NORMAL;
			memcpy(obj_cur->trans[3], last_normal_pos,
				sizeof(float) * 3);
		}
		break;

	default:
		break;
	}
	key_g_last = key_g_now;
	right_click_last = right_click_now;

	glfwGetCursorPos(window, m->last_pos + 0, m->last_pos + 1);
}
