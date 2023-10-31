#include <malloc.h>
#include <string.h>
#include <GL/glew.h>

#include "mesh.h"

static void _mesh_setup_buffers(mesh_t *m)
{
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	glGenBuffers(1, &m->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		       sizeof(vertex_t), (void *)offsetof(vertex_t, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		       sizeof(vertex_t), (void *)offsetof(vertex_t, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		       sizeof(vertex_t), (void *)offsetof(vertex_t, norm));
	glBufferData(GL_ARRAY_BUFFER,
	      sizeof(vertex_t) * m->num_verts, m->verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	      sizeof(uint16_t) * m->num_indis, m->indis, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

mesh_t *mesh_create_axis(void)
{
	const vertex_t verts[6] = {
		{{0, 0, 0}, {0, 0}, {1, 0, 0}},
		{{1, 0, 0}, {0, 0}, {1, 0, 0}},
		{{0, 0, 0}, {0, 0}, {0, 1, 0}},
		{{0, 1, 0}, {0, 0}, {0, 1, 0}},
		{{0, 0, 0}, {0, 0}, {0, 0, 1}},
		{{0, 0, 1}, {0, 0}, {0, 0, 1}},
	};

	const uint16_t indis[6] = {
		0, 1,
		2, 3,
		4, 5,
	};

	return (mesh_create_data(6, 6, verts, indis));
}

mesh_t *mesh_create_data(const uint16_t num_verts, const uint16_t num_indis,
			 const vertex_t *verts, const uint16_t *indis)
{
	mesh_t *m = malloc(sizeof(mesh_t));

	const size_t verts_size = sizeof(vertex_t) * num_verts;
	const size_t indis_size = sizeof(uint16_t) * num_indis;

	m->num_verts = num_verts;
	m->verts = malloc(verts_size);
	memcpy(m->verts, verts, verts_size);

	m->num_indis = num_indis;
	m->indis = malloc(indis_size);
	memcpy(m->indis, indis, indis_size);

	_mesh_setup_buffers(m);

	return (m);
}

void mesh_draw(mesh_t *m)
{
	glBindVertexArray(m->vao);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawElements(GL_TRIANGLES, m->num_indis, GL_UNSIGNED_SHORT, m->indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void mesh_destroy(mesh_t *m)
{
	m->num_verts = 0;
	m->num_indis = 0;
	glDeleteVertexArrays(1, &m->vao);
	glDeleteBuffers(1, &m->vbo);
	glDeleteBuffers(1, &m->ebo);
	free(m->verts);
	free(m->indis);
	free(m);
}
