#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <GL/glew.h>

#include "util.h"
#include "debug.h"
#include "scene.h"

static void _scene_read_mesh(mesh_t *m, FILE *f)
{
	fread(&m->num_verts, sizeof(uint16_t), 1, f);
	m->num_verts = uint16_endian_flip(m->num_verts);
	m->verts = malloc(sizeof(vertex_t) * m->num_verts);
	for (int i = 0; i < m->num_verts; i++)
	{
		fread(m->verts[i].pos, sizeof(float), 3, f);
		m->verts[i].pos[0] = float_endian_flip(m->verts[i].pos[0]);
		m->verts[i].pos[1] = float_endian_flip(m->verts[i].pos[1]);
		m->verts[i].pos[2] = float_endian_flip(m->verts[i].pos[2]);

		fread(m->verts[i].uv, sizeof(float), 2, f);
		m->verts[i].uv[0] = float_endian_flip(m->verts[i].uv[0]);
		m->verts[i].uv[1] = float_endian_flip(m->verts[i].uv[1]);

		fread(m->verts[i].norm, sizeof(float), 3, f);
		m->verts[i].norm[0] = float_endian_flip(m->verts[i].norm[0]);
		m->verts[i].norm[1] = float_endian_flip(m->verts[i].norm[1]);
		m->verts[i].norm[2] = float_endian_flip(m->verts[i].norm[2]);
	}

	fread(&m->num_indis, sizeof(uint16_t), 1, f);
	m->num_indis = uint16_endian_flip(m->num_indis);
	m->indis = malloc(sizeof(uint16_t) * m->num_indis);
	for (int i = 0; i < m->num_indis; i++)
	{
		fread(m->indis + i, sizeof(uint16_t), 1, f);
		m->indis[i] = uint16_endian_flip(m->indis[i]);
	}

	mesh_gen_buffers(m);
}

static void _scene_read_aabb(aabb_t *box, FILE *f)
{
	fread(box, sizeof(float), 6, f);
	box->xmin = float_endian_flip(box->xmin);
	box->xmax = float_endian_flip(box->xmax);
	box->ymin = float_endian_flip(box->ymin);
	box->ymax = float_endian_flip(box->ymax);
	box->zmin = float_endian_flip(box->zmin);
	box->zmax = float_endian_flip(box->zmax);
}

static void _scene_read_trans(float *trans, FILE *f)
{
	fread(trans, sizeof(float), 16, f);
	for (int i = 0; i < 16; i++)
		trans[i] = float_endian_flip(trans[i]);
}

static void _scene_read_object(object_t **o, FILE *f)
{
	object_t *obj = (*o = malloc(sizeof(object_t)));

	fread(obj->name, sizeof(char), CONF_NAME_MAX, f);
	obj->mesh = malloc(sizeof(mesh_t));
	_scene_read_mesh(obj->mesh, f);
	_scene_read_aabb(&obj->aabb, f);
	_scene_read_trans((float *)obj->trans, f);
	fread(&obj->flags, sizeof(uint32_t), 1, f);
	obj->flags = uint32_endian_flip(obj->flags);
}

scene_t *scene_read_file(const char *path)
{
	FILE *f = fopen(path, "rb");

	if (!f)
	{
		debugf("Failed to load scene from '%s'\n", path);
		exit(1);
	}

	scene_t *s = malloc(sizeof(scene_t));

	fread(&s->num_objects, sizeof(uint16_t), 1, f);
	s->num_objects = uint16_endian_flip(s->num_objects);
	s->objects = malloc(sizeof(object_t *) * s->num_objects);
	for (int i = 0; i < s->num_objects; i++)
		_scene_read_object(s->objects + i, f);

	debugf("Loaded Scene from '%s' (num_objects=%d)\n",
	path, s->num_objects);

	return (s);
}
