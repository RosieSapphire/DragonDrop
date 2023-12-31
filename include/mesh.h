#ifndef ENGINE_MESH_H_
#define ENGINE_MESH_H_

#include <stdint.h>
#include <stdbool.h>

#include "vertex.h"

typedef struct
{
	uint16_t num_verts, num_indis;
	vertex_t *verts;
	uint16_t *indis;
	uint32_t vao, vbo, ebo;
} mesh_t;

void mesh_gen_buffers(mesh_t *m);
mesh_t *mesh_create_axis(void);
mesh_t *mesh_create_data(const uint16_t num_verts, const uint16_t num_indis,
			 const vertex_t *verts, const uint16_t *indis);
void mesh_draw(mesh_t *m, bool cull_backface);
void mesh_destroy(mesh_t *m);

#endif /* ENGINE_MESH_H_ */
