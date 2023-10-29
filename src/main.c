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

static GLFWwindow *window = NULL;
static struct nk_context *ctx = NULL;
static struct nk_glfw glfw = {0};

static char load_buf[CONF_LOAD_BUF_MAX];

static scene_t scene;
static uint32_t shader = 0;
static float proj_mat[4][4] = {{0}};
static float view_mat[4][4] = {{0}};

static double view_angle[2] = {0, 0};
static float focus[3] = {0, 0, 0};
static float zoom = 3.0f;

static int cull_backface = 0;

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

	shader = shader_create("res/base.vert", "res/base.frag");
	mat4_perspective(proj_mat, 75.0f, CONF_ASPECT, CONF_NEAR, CONF_FAR);

	strncpy(load_buf, "pistol.glb", CONF_LOAD_BUF_MAX);
	// memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	
	scene_init(&scene);
}

static void panel_props(void)
{
	static const int width = 210;
	if(!nk_begin(ctx, object_selected ? object_selected->name : "null",
	      nk_rect(0, 0, width, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);

	/* loading object */
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, load_buf,
				CONF_LOAD_BUF_MAX, nk_filter_default);
	if(nk_button_label(ctx, "Load Object")) {
		scene_object_add(&scene, load_buf);
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	}

	/*
	if(node_selected) {
		nk_layout_row_begin(ctx, NK_STATIC, 30, 1);
		nk_layout_row_push(ctx, 50);
		nk_label(ctx, "Position", NK_TEXT_LEFT);
		nk_slider_float(ctx, 0, &node_selected->mat[3][0], 10.0f, 0.1f);
		nk_slider_float(ctx, 0, &node_selected->mat[3][1], 10.0f, 0.1f);
		nk_slider_float(ctx, 0, &node_selected->mat[3][2], 10.0f, 0.1f);
		nk_layout_row_end(ctx);
	}
	*/

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

	const float cosv1 = cosf(view_angle[1]);
	float eye[3] = {
		cosf(view_angle[0]) * cosv1 * zoom,
		sinf(view_angle[0]) * cosv1 * zoom,
	     	sinf(view_angle[1]) * zoom,
	};
	mat4_lookat(view_mat, eye, focus, (float[3]){0, 0, 1});

	for(int i = 0; i < scene.num_objects; i++) {
		object_t **obj = scene.objects + i;
		object_draw(*obj, shader, proj_mat,
	      view_mat, object_selected == *obj);
	}

	nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON,
			CONF_VERT_BUF_MAX, CONF_ELEM_BUF_MAX);
	glfwSwapBuffers(window);
}

static void mouse_input(void)
{
	static double mouse_last[2] = {0, 0};

	/* orbiting */
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
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
		return;
	}

	/* zooming */
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) {
		double mouse_y_now;
		glfwGetCursorPos(window, NULL, &mouse_y_now);
		double mouse_y_delta = mouse_y_now - mouse_last[1];
		mouse_last[1] = mouse_y_now;
		zoom += mouse_y_delta * 0.01f;
		zoom = fmaxf(zoom, 0);
		return;
	}

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
