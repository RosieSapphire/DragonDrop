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
#include "debug.h"
#include "camera.h"
#include "mouse.h"
#include "panels.h"

static GLFWwindow *window;
static struct nk_context *ctx;
static struct nk_glfw glfw = {0};
static mouse_t mouse;
static camera_t cam;

static object_t *obj_cur;

static uint32_t shader;
static uint32_t axis_shader;
static float proj_mat[4][4] = {{0}};
static float view_mat[4][4] = {{0}};

static mesh_t *axis_mesh;

static void init(void)
{
	debug_init();

	obj_cur = NULL;
	cull_backface = false;
	camera_init(&cam);
	mouse_init(&mouse);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(CONF_WIDTH, CONF_HEIGHT,
			   CONF_NAME, NULL, NULL);
	glfwMakeContextCurrent(window);
	debugf("Setup GLFW Context\n");
	glewExperimental = 1;
	glewInit();
	glViewport(0, 0, CONF_WIDTH, CONF_HEIGHT);
	debugf("Setup GLEW\n");
	ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

	struct nk_font_atlas *atlas;

	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);
	debugf("Setup Nuklear\n");

	mat4_perspective(proj_mat, 75.0f, CONF_ASPECT, CONF_NEAR, CONF_FAR);
	scene = scene_create_empty();
	shader = shader_create("res/shaders/base.vert", "res/shaders/base.frag");
	axis_shader = shader_create("res/shaders/axis.vert",
			     "res/shaders/axis.frag");
	axis_mesh = mesh_create_axis();

	aabb_init();
	panels_init();
}

static void axis_draw(void)
{
	const uint32_t proj_loc = glGetUniformLocation(axis_shader, "u_proj");
	const uint32_t view_loc = glGetUniformLocation(axis_shader, "u_view");

	glDisable(GL_CULL_FACE);
	glUseProgram(axis_shader);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (const float *)proj_mat);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (const float *)view_mat);

	glBindVertexArray(axis_mesh->vao);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawElements(GL_LINES, axis_mesh->num_indis,
		GL_UNSIGNED_SHORT, axis_mesh->indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

static void draw(void)
{
	nk_glfw3_new_frame(&glfw);
	panel_props(ctx, &obj_cur);
	panel_list(ctx, &obj_cur);
	debug_panel(ctx);

	if (cull_backface)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	} else
	{
		glDisable(GL_CULL_FACE);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float eye[3];
	float up[3] = {0, 0, 1};

	camera_eye(&cam, eye);
	mat4_lookat(view_mat, eye, cam.focus, up);

	axis_draw();
	for (int i = 0; i < scene->num_objects; i++)
	{
		object_t *obj = scene->objects[i];

		object_draw(obj, shader, proj_mat,
	      view_mat, obj_cur == obj);
		aabb_draw(obj->aabb, proj_mat, view_mat, obj->trans);
	}

	nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON,
			CONF_VERT_BUF_MAX, CONF_ELEM_BUF_MAX);
	glfwSwapBuffers(window);
}

int main(void)
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		mouse_input(&mouse, &cam, window, obj_cur);
		draw();
	}

	glfwTerminate();
	return (0);
}
