#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "mesh.h"
#include "aabb.h"

#define OBJ_HAS_COLLISION 0b00000001
#define OBJ_IS_VISIBLE    0b00000010
#define OBJ_IS_PICKUP     0b00000100

typedef struct {
	char name[CONF_NAME_MAX];
	mesh_t *mesh;
	aabb_t aabb;
	float trans[4][4];
	uint32_t flags;
} object_t;

extern object_t *object_selected;

object_t *object_create_empty(void);
object_t *object_create_tri(void);
object_t *object_create_file(const char *path);
void object_draw(const object_t *obj, const uint32_t shader,
		 float proj_mat[4][4], float view_mat[4][4], bool is_selected);
void object_destroy(object_t *o);

#endif /* OBJECT_H_ */
