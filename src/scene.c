#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <GL/glew.h>

#include "mat4.h"
#include "util.h"
#include "debug.h"
#include "scene.h"

scene_t *scene_create_empty(void)
{
	scene_t *s = malloc(sizeof(scene_t));

	s->num_objects = 0;
	s->objects = malloc(0);
	debugf("Setup Empty Scene.\n");
	return (s);
}

void scene_object_add(scene_t *s, const char *path)
{
	object_t **cur = s->objects + ++s->num_objects - 1;

	s->objects = realloc(s->objects, sizeof(object_t *) * s->num_objects);
	*cur = object_load_file(path);
	if (*cur == NULL)
		s->objects = realloc(s->objects,
		       sizeof(object_t *) * --s->num_objects);
}
