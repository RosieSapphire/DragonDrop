#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "config.h"
#include "util.h"
#include "object.h"
#include "shader.h"
#include "scene.h"
#include "object.h"
#include "mat4.h"
#include "aabb.h"

static GLFWwindow *window = NULL;
static struct nk_context *ctx = NULL;
static struct nk_glfw glfw = {0};

static char load_buf[CONF_LOAD_BUF_MAX];

static scene_t scene;
static uint32_t shader = 0;
static uint32_t axis_shader = 0;
static float proj_mat[4][4] = {{0}};
static float view_mat[4][4] = {{0}};

static float view_angle[2] = {0, 0};
static float focus[3] = {0, 0, 0};
static float zoom = 3.0f;

static int cull_backface = 0;

static mesh_t *axis_mesh = NULL;

static void init(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(CONF_WIDTH, CONF_HEIGHT,
			   CONF_NAME, NULL, NULL);
	glfwMakeContextCurrent(window);
	
	glewExperimental = 1;
	glewInit();
	glViewport(0, 0, CONF_WIDTH, CONF_HEIGHT);
	ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

	struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);

	shader = shader_create("res/shaders/base.vert", "res/shaders/base.frag");
	mat4_perspective(proj_mat, 75.0f, CONF_ASPECT, CONF_NEAR, CONF_FAR);

	strncpy(load_buf, "res/models/pistol.glb", CONF_LOAD_BUF_MAX);
	// memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	
	scene_init(&scene);

	axis_shader =
		shader_create("res/shaders/axis.vert", "res/shaders/axis.frag");
	const vertex_t axis_verts[6] = {
		{{0, 0, 0}, {0, 0}, {1, 0, 0}},
		{{1, 0, 0}, {0, 0}, {1, 0, 0}},
		{{0, 0, 0}, {0, 0}, {0, 1, 0}},
		{{0, 1, 0}, {0, 0}, {0, 1, 0}},
		{{0, 0, 0}, {0, 0}, {0, 0, 1}},
		{{0, 0, 1}, {0, 0}, {0, 0, 1}},
	};

	const uint16_t axis_indis[6] = {
		0, 1,
		2, 3,
		4, 5,
	};
	axis_mesh = mesh_create_data(6, 6, axis_verts, axis_indis);

	aabb_init();
}

static void panel_props(void)
{
	static const int width = 420;
	if(!nk_begin(ctx, object_selected ? object_selected->name : "null",
	      nk_rect(0, 0, width, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);

	/* loading object */
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, load_buf,
				CONF_LOAD_BUF_MAX, nk_filter_default);
	/* TODO: Make sure to add instancing at some point */
	if(nk_button_label(ctx, "Load Object")) {
		scene_object_add(&scene, load_buf);
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	}

	if(object_selected) {
		nk_layout_row_dynamic(ctx, 30, 4);
		nk_label(ctx, "Position", NK_TEXT_LEFT);
		nk_property_float(ctx, "X", -INFINITY,
		    &object_selected->trans[3][0], INFINITY, 0.001f, 0.001f);
		nk_property_float(ctx, "Y", -INFINITY,
		    &object_selected->trans[3][1], INFINITY, 0.001f, 0.001f);
		nk_property_float(ctx, "Z", -INFINITY,
		    &object_selected->trans[3][2], INFINITY, 0.001f, 0.001f);

		nk_layout_row_dynamic(ctx, 30, 1);
		nk_checkbox_flags_label(ctx, "Has Collision",
			  &object_selected->flags, OBJ_HAS_COLLISION);
		nk_checkbox_flags_label(ctx, "Is Visible",
			  &object_selected->flags, OBJ_IS_VISIBLE);
	}


	nk_label(ctx, "Global", NK_TEXT_LEFT);
	nk_checkbox_label(ctx, "Backface Culling", &cull_backface);
	nk_end(ctx);
}

static void panel_list(void)
{
	static const int width = 240;
	if(!nk_begin(ctx, "Object List", nk_rect(CONF_WIDTH - width, 0, width,
					CONF_HEIGHT), NK_WINDOW_TITLE |
		NK_WINDOW_BORDER))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);
	for(int i = 0; i < scene.num_objects; i++) {
		object_t *obj = scene.objects[i];
		if(nk_option_label(ctx, obj->name, obj == object_selected)) {
			object_selected = obj;
		}
	}

	nk_end(ctx);
}

static void axis_draw(void)
{
	glDisable(GL_CULL_FACE);
	glUseProgram(axis_shader);
	const uint32_t proj_loc = glGetUniformLocation(axis_shader, "u_proj");
	const uint32_t view_loc = glGetUniformLocation(axis_shader, "u_view");
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (const float *)proj_mat);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (const float *)view_mat);

	glBindVertexArray(axis_mesh->vao);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawElements(GL_LINES, axis_mesh->num_indis,
		GL_UNSIGNED_SHORT, axis_mesh->indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

static void camera_eye(float *out)
{
	const float cosv1 = cosf(view_angle[1]);
	out[0] = cosf(view_angle[0]) * cosv1 * zoom;
	out[1] = sinf(view_angle[0]) * cosv1 * zoom;
	out[2] = sinf(view_angle[1]) * zoom;
}

static void draw(void)
{
	nk_glfw3_new_frame(&glfw);
	panel_props();
	panel_list();

	if(cull_backface) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	} else {
		glDisable(GL_CULL_FACE);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float eye[3];
	camera_eye(eye);
	mat4_lookat(view_mat, eye, focus, (float[3]){0, 0, 1});

	axis_draw();
	for(int i = 0; i < scene.num_objects; i++) {
		object_t **obj = scene.objects + i;
		object_draw(*obj, shader, proj_mat,
	      view_mat, object_selected == *obj);
		aabb_draw((*obj)->aabb, proj_mat, view_mat, (*obj)->trans);
	}

	nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON,
			CONF_VERT_BUF_MAX, CONF_ELEM_BUF_MAX);
	glfwSwapBuffers(window);
}

enum input_mode {
	IMODE_NORMAL,
	IMODE_MOVE,
	IMODE_ROTATE,
	IMODE_SCALE,
	IMODE_NUM_STATES,
};

static enum input_mode input_mode = IMODE_NORMAL;
static double mouse_last[2] = {0, 0};

static void mouse_input_orbiting(void)
{
	if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
		return;

	double mouse_now[2];
	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);
	double mouse_delta[2] = {
		mouse_now[0] - mouse_last[0],
		mouse_now[1] - mouse_last[1],
	};
	mouse_last[0] = mouse_now[0];
	mouse_last[1] = mouse_now[1];
	view_angle[0] += mouse_delta[0] * 0.02f;
	view_angle[1] += mouse_delta[1] * 0.02f;
	view_angle[1] = clampf(view_angle[1], -1.5f, 1.5f);
}

static void mouse_input_zooming(void)
{
	if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
		return;

	double mouse_y_now;
	glfwGetCursorPos(window, NULL, &mouse_y_now);
	double mouse_y_delta = mouse_y_now - mouse_last[1];
	mouse_last[1] = mouse_y_now;
	zoom += mouse_y_delta * 0.01f;
	zoom = fmaxf(zoom, 0);
}

static void camera_forw_side(float *eye, float *forw, float *side, float *up)
{
	vector_subtract(focus, eye, forw, 3);
	vector_normalize(forw, 3);
	vector3_cross(forw, up, side);
}

static void mouse_input_panning(void)
{
	if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		return;
	}

	double mouse_now[2];
	glfwGetCursorPos(window, mouse_now + 0, mouse_now + 1);

	if(mouse_now[0] <= 420 || mouse_now[0] >= CONF_WIDTH - 240) {
		return;
	}

	double mouse_delta[2] = {
		mouse_now[0] - mouse_last[0],
		mouse_now[1] - mouse_last[1],
	};

	float eye[3];
	camera_eye(eye);
	float up[3] = {0, 0, 1}, forw[3], side[3];
	camera_forw_side(eye, forw, side, up);
	float move[3];
	vector_scale(up, mouse_delta[1] * 0.02f, 3);
	vector_scale(side, mouse_delta[0] * 0.02f, 3);
	vector_add(up, side, move, 3);

	vector_add(focus, move, focus, 3);
}

static int axis_move = -1;
static void mouse_input_moving(void)
{
	static int axis_key_last[3] = {0, 0, 0};
	int axis_key_now[3] = {
		glfwGetKey(window, GLFW_KEY_X),
		glfwGetKey(window, GLFW_KEY_Y),
		glfwGetKey(window, GLFW_KEY_Z),
	};

	for(int i = 0; i < 3; i++) {
		if(!axis_key_last[i] && axis_key_now[i]) {
			if(i == axis_move) {
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

	float cam_eye[3];
	float forw[3];
	float side[3];
	float up[3] = {0, 0, 1};
	camera_eye(cam_eye);
	camera_forw_side(cam_eye, forw, side, up);
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

	printf("%d\n", axis_move);

	vector_add(obj_pos, move, obj_pos, 3);
}

static void mouse_input(void)
{
	static int key_g_last = 0;
	static int right_click_last = 0;
	int key_g_now = glfwGetKey(window, GLFW_KEY_G);
	int right_click_now =
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	static float last_normal_pos[3] = {0, 0, 0};
	switch(input_mode) {
	case IMODE_NORMAL:
		mouse_input_orbiting();
		mouse_input_zooming();
		mouse_input_panning();
		if(key_g_now && !key_g_last && object_selected) {
			vector_copy(object_selected->trans[3],
				last_normal_pos, 3);
			input_mode = IMODE_MOVE;
		}
		break;

	case IMODE_MOVE:
		mouse_input_moving();
		if(key_g_now && !key_g_last) {
			axis_move = -1;
			input_mode = IMODE_NORMAL;
		}

		if((right_click_now && !right_click_last) ||
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

static void terminate(void)
{
	glfwTerminate();
}

int main(void)
{
	init();

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		mouse_input();
		draw();
	}

	terminate();
	return 0;
}
