#ifndef SCENE_H_
#define SCENE_H_

#include "object.h"

typedef struct {
	uint16_t num_objects;
	object_t **objects;
} scene_t;

void scene_init(scene_t *s);
void scene_object_add(scene_t *s, const char *path);

#endif /* SCENE_H_ */
