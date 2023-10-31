#ifndef SCENE_H_
#define SCENE_H_

#include "object.h"

typedef struct
{
	uint16_t num_objects;
	object_t **objects;
} scene_t;

/* general */
scene_t *scene_create_empty(void);
void scene_object_add(scene_t *s, const char *path);

/* write */
void scene_write_file(const scene_t *s, const char *path);

/* read */
scene_t *scene_read_file(const char *path);

#endif /* SCENE_H_ */
