#include <malloc.h>
#include <string.h>
#include <GL/glew.h>

#include "mesh.h"

mesh_t *mesh_create_tri(void)
{
	mesh_t *m = malloc(sizeof(mesh_t));
	m->num_verts = 3;
	m->verts = malloc(m->num_verts * sizeof(vertex_t));
	m->verts[0].pos[0]  = -0.5f;
	m->verts[0].pos[1]  =  0.0f;
	m->verts[0].pos[2]  = -0.5f;
	m->verts[0].uv[0]   =  0.0f;
	m->verts[0].uv[1]   =  0.0f;
	m->verts[0].norm[0] =  0.0f;
	m->verts[0].norm[1] =  1.0f;
	m->verts[0].norm[2] =  0.0f;

	m->verts[1].pos[0]  =  0.5f;
	m->verts[1].pos[1]  =  0.0f;
	m->verts[1].pos[2]  = -0.5f;
	m->verts[1].uv[0]   =  1.0f;
	m->verts[1].uv[1]   =  0.0f;
	m->verts[1].norm[0] =  0.0f;
	m->verts[1].norm[1] =  1.0f;
	m->verts[1].norm[2] =  0.0f;

	m->verts[2].pos[0]  =  0.0f;
	m->verts[2].pos[1]  =  0.0f;
	m->verts[2].pos[2]  =  0.5f;
	m->verts[2].uv[0]   =  0.5f;
	m->verts[2].uv[1]   =  1.0f;
	m->verts[2].norm[0] =  0.0f;
	m->verts[2].norm[1] =  1.0f;
	m->verts[2].norm[2] =  0.0f;

	m->num_indis = 3;
	m->indis = malloc(m->num_indis * sizeof(uint16_t));
	m->indis[0] = 0;
	m->indis[1] = 1;
	m->indis[2] = 2;

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

	return m;
}

mesh_t *mesh_create_data(const uint16_t num_verts, const uint16_t num_indis,
			 const vertex_t *verts, const uint16_t *indis)
{
	mesh_t *m = malloc(sizeof(mesh_t));

	m->num_verts = num_verts;
	const size_t verts_size = sizeof(vertex_t) * num_verts;
	m->verts = malloc(verts_size);
	memcpy(m->verts, verts, verts_size);

	m->num_indis = num_indis;
	const size_t indis_size = sizeof(uint16_t) * num_indis;
	m->indis = malloc(indis_size);
	memcpy(m->indis, indis, indis_size);

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
	return m;
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
