#ifndef NODE_H_
#define NODE_H_

#include <stdint.h>

#include "config.h"

typedef struct node_t node_t;
struct node_t {
	char name[CONF_NAME_MAX];
	float mat[4][4];
	uint16_t obj_index, num_children;
	node_t *parent, *children;
};

extern node_t *node_selected;

void node_nk_tree(void *ctxin, const node_t *node);
void node_add(node_t *parent, uint16_t obj_index, const char *name);
node_t *node_from_obj_ind(node_t *start, const uint16_t obj_ind);
// void node_remove(node_t *node);

#endif /* NODE_H_ */
