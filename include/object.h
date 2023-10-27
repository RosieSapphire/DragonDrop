#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdint.h>

#include "mesh.h"

enum object_flags {
	OBJ_IS_ACTIVE  = 0b00000001,
	OBJ_IS_VISIBLE = 0b00000010,
};

typedef struct {
	mesh_t *mesh;
	enum object_flags flags;
} object_t;

object_t *object_gen_tri(void);
// object_t *object_load_file(const char *path);
void object_destroy(object_t *o);

#endif /* OBJECT_H_ */
