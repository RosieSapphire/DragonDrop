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

static GLFWwindow *window = NULL;
static struct nk_context *ctx = NULL;
static struct nk_glfw glfw = {0};

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
}

static void panel_left_tree(void)
{
	if(!nk_tree_push(ctx, NK_TREE_NODE, "Test Tree", NK_WINDOW_IS_COLLAPSED))
		return;
}

static void panel_left_group(void)
{
	if(!nk_group_begin(ctx, "Test Group", NK_WINDOW_TITLE))
		return;

	nk_group_end(ctx);
}

static void panel_left(void)
{
	if(!nk_begin(ctx, "Left Panel",
	      nk_rect(0, 0, CONF_WIDTH >> 3, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 2);
	(void)nk_button_label(ctx, "Test Button 1");
	(void)nk_button_label(ctx, "Test Button 2");

	nk_layout_row_dynamic(ctx, 60, 1);
	panel_left_group();
	nk_end(ctx);
}

static void panel_right(void)
{
	if(!nk_begin(ctx, "Right Panel",
	      nk_rect(CONF_WIDTH - (CONF_WIDTH >> 3),
	       0, CONF_WIDTH >> 3, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);
	(void)nk_button_label(ctx, "Test Button 3");
	(void)nk_button_label(ctx, "Test Button 4");
	nk_end(ctx);
}

static void draw(void)
{
	nk_glfw3_new_frame(&glfw);
	panel_left();
	panel_right();

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON,
			CONF_VERT_BUF_MAX, CONF_ELEM_BUF_MAX);
	glfwSwapBuffers(window);
}

static void input(void)
{
	glfwPollEvents();
	if(glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void terminate(void)
{
	glfwTerminate();
}

int main(void)
{
	init();

	while(!glfwWindowShouldClose(window)) {
		draw();
		input();
	}

	terminate();
	return 0;
}
