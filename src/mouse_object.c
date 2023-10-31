#include "object.h"
#include "vector.h"
#include "mouse.h"

static void _mouse_input_moving_axis_key(mouse_t *m, GLFWwindow *window)
{
	static int axis_key_last[3] = {0, 0, 0};
	int axis_key_now[3] = {
		glfwGetKey(window, GLFW_KEY_X),
		glfwGetKey(window, GLFW_KEY_Y),
		glfwGetKey(window, GLFW_KEY_Z),
	};

	for (int i = 0; i < 3; i++)
	{
		if (!axis_key_last[i] && axis_key_now[i])
		{
			if (i == m->axis_move)
				m->axis_move = -1;
			else
				m->axis_move = i;
		}
		axis_key_last[i] = axis_key_now[i];
	}
}

void mouse_object_moving(mouse_t *m, camera_t *c,
			 GLFWwindow *window, object_t *obj_cur)
{
	_mouse_input_moving_axis_key(m, window);

	double mouse_now[2];
	double mouse_delta[2];

	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);
	mouse_get_delta(m, window, mouse_now, mouse_delta);

	float cam_eye[3], forw[3], side[3], up[3], move[3];

	camera_eye(c, cam_eye);
	camera_forw_side_up(c, cam_eye, forw, side, up);
	vector_scale(up, -mouse_delta[1] * 0.02f, 3);
	vector_scale(side, -mouse_delta[0] * 0.02f, 3);
	vector_add(up, side, move, 3);

	static const float move_axis[3][3] = {
		{1, 0, 0}, {0, 1, 0}, {0, 0, 1}
	};

	if (m->axis_move != -1)
		for (int i = 0; i < 3; i++)
			move[i] *= move_axis[m->axis_move][i];

	vector_add(obj_cur->trans[3], move, obj_cur->trans[3], 3);
}
