#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <GL/glew.h>

#include "mat4.h"
#include "util.h"
#include "debug.h"
#include "scene.h"

scene_t *scene;

scene_t *scene_create_empty(void)
{
	scene_t *s = malloc(sizeof(scene_t));
	s->num_objects = 0;
	s->objects = malloc(0);
	debugf("Setup Empty Scene.\n");
	return s;
}

static void _scene_read_mesh(mesh_t *m, FILE *f)
{
	fread(&m->num_verts, sizeof(uint16_t), 1, f);
	m->num_verts = uint16_endian_flip(m->num_verts);
	m->verts = malloc(sizeof(vertex_t) * m->num_verts);
	for(int i = 0; i < m->num_verts; i++) {
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
	for(int i = 0; i < m->num_indis; i++) {
		fread(m->indis + i, sizeof(uint16_t), 1, f);
		m->indis[i] = uint16_endian_flip(m->indis[i]);
	}

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
	for(int i = 0; i < 16; i++) {
		trans[i] = float_endian_flip(trans[i]);
	}
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

scene_t *scene_create_file(__attribute__((unused))const char *path)
{
	FILE *f = fopen(path, "rb");
	if(!f) {
		debugf("Failed to load scene from '%s'\n", path);
		exit(1);
	}

	scene_t *s = malloc(sizeof(scene_t));
	fread(&s->num_objects, sizeof(uint16_t), 1, f);
	s->num_objects = uint16_endian_flip(s->num_objects);
	s->objects = malloc(sizeof(object_t *) * s->num_objects);
	for(int i = 0; i < s->num_objects; i++) {
		_scene_read_object(s->objects + i, f);
	}

	debugf("Loaded Scene from '%s' (num_objects=%d)\n",
	path, s->num_objects);

	return s;
}

static void _scene_write_mesh(mesh_t *m, FILE *f)
{
	const uint16_t num_verts_flip = uint16_endian_flip(m->num_verts);
	fwrite(&num_verts_flip, sizeof(uint16_t), 1, f);
	for(int i = 0; i < m->num_verts; i++) {
		const float pos_flip[3] = {
			float_endian_flip(m->verts[i].pos[0]),
			float_endian_flip(m->verts[i].pos[1]),
			float_endian_flip(m->verts[i].pos[2]),
		};

		fwrite(pos_flip, sizeof(float), 3, f);

		const float uv_flip[2] = {
			float_endian_flip(m->verts[i].uv[0]),
			float_endian_flip(m->verts[i].uv[1]),
		};

		fwrite(uv_flip, sizeof(float), 2, f);

		const float norm_flip[3] = {
			float_endian_flip(m->verts[i].norm[0]),
			float_endian_flip(m->verts[i].norm[1]),
			float_endian_flip(m->verts[i].norm[2]),
		};

		fwrite(norm_flip, sizeof(float), 3, f);
	}

	const uint16_t num_indis_flip = uint16_endian_flip(m->num_indis);
	fwrite(&num_indis_flip, sizeof(uint16_t), 1, f);
	for(int i = 0; i < m->num_indis; i++) {
		const uint16_t indi_flip = uint16_endian_flip(m->indis[i]);
		fwrite(&indi_flip, sizeof(uint16_t), 1, f);
	}
}

static void _scene_write_aabb(const aabb_t box, FILE *f)
{
	aabb_t box_flip = {
		float_endian_flip(box.xmin), float_endian_flip(box.xmax),
		float_endian_flip(box.ymin), float_endian_flip(box.ymax),
		float_endian_flip(box.zmin), float_endian_flip(box.zmax),
	};
	fwrite(&box_flip, sizeof(float), 6, f);
}

static void _scene_write_trans(float *trans, FILE *f)
{
	float t[4][4];
	mat4_copy(trans, (float *)t);
	for(int i = 0; i < 16; i++) {
		((float *)t)[i] = float_endian_flip(*((float *)t + i));
	}
	fwrite(t, sizeof(float), 16, f);
}

static void _scene_write_object(const object_t *o, FILE *f)
{
	fwrite(o->name, sizeof(char), CONF_NAME_MAX, f);
	_scene_write_mesh(o->mesh, f);
	_scene_write_aabb(o->aabb, f);
	_scene_write_trans((float *)o->trans, f);
	const uint32_t flags_flip = uint32_endian_flip(o->flags);
	fwrite(&flags_flip, sizeof(uint32_t), 1, f);
}

void scene_write_file(const scene_t *s, const char *path)
{
	FILE *out = fopen(path, "wb");
	if(out) {
		fclose(out);
		remove(path);
		out = fopen(path, "wb");
	}

	const uint16_t num_objects_flip = uint16_endian_flip(s->num_objects);
	fwrite(&num_objects_flip, sizeof(uint16_t), 1, out);
	for(int i = 0; i < s->num_objects; i++) {
		_scene_write_object(s->objects[i], out);
	}
}

void scene_object_add(scene_t *s, const char *path)
{
	object_t **cur = s->objects + ++s->num_objects - 1;
	s->objects = realloc(s->objects, sizeof(object_t *) * s->num_objects);
	*cur = object_create_file(path);
	if(*cur == NULL) {
		s->objects = realloc(s->objects,
		       sizeof(object_t *) * --s->num_objects);
	}
}
