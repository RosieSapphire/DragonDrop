#include <stdio.h>
#include <string.h>

#include "util.h"
#include "debug.h"
#include "scene.h"

static void _scene_write_mesh(mesh_t *m, FILE *f)
{
	const uint16_t num_verts_flip = uint16_endian_flip(m->num_verts);

	fwrite(&num_verts_flip, sizeof(uint16_t), 1, f);
	for (int i = 0; i < m->num_verts; i++)
	{
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
	for (int i = 0; i < m->num_indis; i++)
	{
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

	memcpy(t, trans, sizeof(float) * 16);
	for (int i = 0; i < 16; i++)
		((float *)t)[i] = float_endian_flip(*((float *)t + i));

	fwrite(t, sizeof(float), 16, f);
}

static void _scene_write_object(const object_t *o, FILE *f)
{
	const uint32_t flags_flip = uint32_endian_flip(o->flags);

	fwrite(o->name, sizeof(char), CONF_NAME_MAX, f);
	_scene_write_mesh(o->mesh, f);
	_scene_write_aabb(o->aabb, f);
	_scene_write_trans((float *)o->trans, f);
	fwrite(&flags_flip, sizeof(uint32_t), 1, f);
}

void scene_write_file(const scene_t *s, const char *path)
{
	FILE *out = fopen(path, "wb");

	if (out)
	{
		fclose(out);
		remove(path);
		out = fopen(path, "wb");
	}

	const uint16_t num_objects_flip = uint16_endian_flip(s->num_objects);

	fwrite(&num_objects_flip, sizeof(uint16_t), 1, out);
	for (int i = 0; i < s->num_objects; i++)
		_scene_write_object(s->objects[i], out);

	debugf("Wrote Scene to '%s' (num_objects=%d)\n", path, s->num_objects);
}

