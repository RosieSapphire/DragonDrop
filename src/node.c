#include <malloc.h>
#include <string.h>

#include "nuklear.h"
#include "object.h"
#include "vector.h"
#include "mat4.h"
#include "node.h"

node_t *node_selected = NULL;

void node_nk_tree(void *ctxin, const node_t *node)
{
	struct nk_context *ctx = ctxin;

	nk_layout_row_dynamic(ctx, 16, 1);
	if(nk_tree_push(ctx, NK_TREE_TAB, node->name, NK_MAXIMIZED)) {
		for(int i = 0; i < node->num_children; i++) {
			node_t *node_child = node->children + i;
			node_nk_tree(ctx, node_child);
		}
		nk_tree_pop(ctx);
	}
}

void node_add(node_t *parent, uint16_t obj_index, const char *name)
{
	const uint16_t objects_size = ++parent->num_children * sizeof(object_t);
	if(!parent->children) {
		parent->children = malloc(objects_size);
	} else {
		parent->children = realloc(parent->children, objects_size);
	}

	node_t *this = parent->children + parent->num_children - 1;
	strncpy(this->name, name, CONF_NAME_MAX);
	mat4_identity(this->mat);
	this->obj_index = obj_index;
	this->num_children = 0;
	this->parent = parent;
	this->children = malloc(0);

	node_selected = this;
}

node_t *node_from_obj_ind(node_t *start, const uint16_t obj_ind)
{
	node_t *ret = NULL;
	if(start->obj_index != obj_ind) {
		for(int i = 0; i < start->num_children; i++) {
			ret = node_from_obj_ind(start->children + i, obj_ind);
		}
	} else {
		ret = start;
	}

	return ret;
}
