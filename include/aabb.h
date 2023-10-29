#ifndef AABB_H_
#define AABB_H_

#include "mesh.h"

/* Axis-Aligned Bounding Box */
typedef struct {
	float xmin, xmax, ymin, ymax, zmin, zmax;
} aabb_t;

void aabb_init(void);
aabb_t aabb_from_mesh(const mesh_t *mesh);
void aabb_draw(const aabb_t box, float proj[4][4],
	       float view[4][4], float model[4][4]);

#endif /* AABB_H_ */
