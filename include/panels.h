#ifndef PANELS_H_
#define PANELS_H_

#include "object.h"
#include "scene.h"

void panels_init(void);
void panel_props(void *ctxin, object_t **obj_cur,
		 scene_t **s, int *cull_backface);
void panel_list(void *ctxin, object_t **obj_cur, scene_t **s);

#endif /* PANELS_H_ */
