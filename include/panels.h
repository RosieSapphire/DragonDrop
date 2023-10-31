#ifndef PANELS_H_
#define PANELS_H_

#include "object.h"

extern int cull_backface;

void panels_init(void);
void panel_props(void *ctxin, object_t **obj_cur);
void panel_list(void *ctxin, object_t **obj_cur);

#endif /* PANELS_H_ */
