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
#include "mat4.h"

static GLFWwindow *window = NULL;
static struct nk_context *ctx = NULL;
static struct nk_glfw glfw = {0};

static char load_buf[CONF_LOAD_BUF_MAX];

static object_t *test_obj = NULL;
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
	mat4_perspective(proj_mat, 90.0f, CONF_ASPECT, CONF_NEAR, CONF_FAR);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

static void panel_props(void)
{
	char label[CONF_NAME_MAX];
	sprintf(label, "%s", test_obj->name);

	if(!nk_begin(ctx, label, nk_rect(0, 0, 210, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);

	/* loading object */
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, load_buf,
				CONF_LOAD_BUF_MAX,
				nk_filter_default);
	if(nk_button_label(ctx, "Load Object") && !test_obj) {
		test_obj = object_create_file(load_buf);
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	}

	nk_checkbox_label(ctx, "Backface Culling", &cull_backface);

	if(nk_button_label(ctx, "Destroy Selected") && test_obj) {
		object_destroy(test_obj);
		test_obj = NULL;
	}

	nk_end(ctx);
}

static void draw(void)
{
	nk_glfw3_new_frame(&glfw);
	panel_props();

	if(cull_backface) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	} else {
		glDisable(GL_CULL_FACE);
	}

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4_lookat(view_mat,
	     (float[3]){
		sinf(view_angle[0]) * cosf(view_angle[1]) * zoom,
		cosf(view_angle[0]) * cosf(view_angle[1]) * zoom,
		sinf(view_angle[1]) * zoom,
	     }, focus, (float[3]){0, 0, 1});

	if(test_obj) {
		mesh_draw(test_obj->mesh, shader, proj_mat, view_mat);
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
		view_angle[0] -= mouse_delta[0] * 0.02f;
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
