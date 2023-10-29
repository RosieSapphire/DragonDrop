#include <malloc.h>
#include <string.h>

#include "scene.h"

void scene_init(scene_t *s)
{
	s->num_objects = 0;
	s->objects = malloc(0);
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
