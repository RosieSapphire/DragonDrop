#include <malloc.h>
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

void mesh_draw(const mesh_t *m, const uint32_t shader,
	       const float proj[4][4], const float view[4][4])
{
	glUseProgram(shader);
	const uint32_t proj_loc = glGetUniformLocation(shader, "u_proj");
	const uint32_t view_loc = glGetUniformLocation(shader, "u_view");
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (const float *)proj);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (const float *)view);
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
