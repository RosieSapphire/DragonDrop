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

#include "object.h"
#include "config.h"

static GLFWwindow *window = NULL;
static struct nk_context *ctx = NULL;
static struct nk_glfw glfw = {0};

static char load_buf[CONF_LOAD_BUF_MAX];

static object_t *test_obj = NULL;

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

static void panel_left(void)
{
	if(!nk_begin(ctx, "Left Panel",
	      nk_rect(0, 0, 210, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);

	if(nk_button_label(ctx, "Generate Triangle")) {
		if(!test_obj)
			test_obj = object_gen_tri();
	}

	if(nk_button_label(ctx, "Destroy Triangle")) {
		if(test_obj) {
			object_destroy(test_obj);
			test_obj = NULL;
		}
	}

	/* loading object */
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, load_buf,
				CONF_LOAD_BUF_MAX,
				nk_filter_default);
	if(nk_button_label(ctx, "Load Object")) {
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	}
	nk_end(ctx);
}

static void draw(void)
{
	nk_glfw3_new_frame(&glfw);
	panel_left();

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
