#include <stdio.h>
#include <math.h>
#include <GL/glew.h>

#include "shader.h"
#include "aabb.h"

static uint32_t vao, vbo, ebo, shader;

void aabb_init(void)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		       sizeof(vertex_t), (void *)offsetof(vertex_t, pos));
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	shader = shader_create("res/shaders/aabb.vert", "res/shaders/aabb.frag");
}

aabb_t aabb_from_mesh(const mesh_t *mesh)
{
	aabb_t box = {0};

	for (int i = 0; i < mesh->num_verts; i++)
	{
		float *pos = mesh->verts[i].pos;

		box.xmin = fminf(box.xmin, pos[0]);
		box.xmax = fmaxf(box.xmax, pos[0]);

		box.ymin = fminf(box.ymin, pos[1]);
		box.ymax = fmaxf(box.ymax, pos[1]);

		box.zmin = fminf(box.zmin, pos[2]);
		box.zmax = fmaxf(box.zmax, pos[2]);
	}

	printf("xmin=%f, xmax=%f\n", box.xmin, box.xmax);
	printf("ymin=%f, ymax=%f\n", box.ymin, box.ymax);
	printf("zmin=%f, zmax=%f\n", box.zmin, box.zmax);

	return (box);
}

static const uint16_t indis[] = {
	0, 1,
	1, 2,
	2, 3,
	0, 3,

	4, 5,
	5, 6,
	6, 7,
	4, 7,

	0, 4,
	1, 5,
	2, 6,
	3, 7
};

static void _aabb_draw_setup(const aabb_t box, float proj[4][4],
			     float view[4][4], float model[4][4])
{
	const vertex_t verts[] = {
		{{box.xmin, box.ymin, box.zmin}, {0, 0}, {0, 0, 0}},
		{{box.xmax, box.ymin, box.zmin}, {0, 0}, {0, 0, 0}},
		{{box.xmax, box.ymax, box.zmin}, {0, 0}, {0, 0, 0}},
		{{box.xmin, box.ymax, box.zmin}, {0, 0}, {0, 0, 0}},

		{{box.xmin, box.ymin, box.zmax}, {0, 0}, {0, 0, 0}},
		{{box.xmax, box.ymin, box.zmax}, {0, 0}, {0, 0, 0}},
		{{box.xmax, box.ymax, box.zmax}, {0, 0}, {0, 0, 0}},
		{{box.xmin, box.ymax, box.zmax}, {0, 0}, {0, 0, 0}},
	};

	glDisable(GL_CULL_FACE);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	      sizeof(indis), indis, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	const uint32_t proj_loc = glGetUniformLocation(shader, "u_proj");
	const uint32_t view_loc = glGetUniformLocation(shader, "u_view");
	const uint32_t model_loc = glGetUniformLocation(shader, "u_model");

	glUseProgram(shader);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (const float *)proj);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (const float *)view);
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (const float *)model);
}

void aabb_draw(const aabb_t box, float proj[4][4],
	       float view[4][4], float model[4][4])
{
	_aabb_draw_setup(box, proj, view, model);

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawElements(GL_LINES, sizeof(indis) / sizeof(*indis),
		GL_UNSIGNED_SHORT, indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
