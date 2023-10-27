#include <malloc.h>

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
	return m;
}

void mesh_destroy(mesh_t *m)
{
	m->num_verts = 0;
	m->num_indis = 0;
	free(m->verts);
	free(m->indis);
	free(m);
}
