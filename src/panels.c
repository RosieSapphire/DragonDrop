#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include "nuklear.h"
#include "config.h"
#include "object.h"
#include "scene.h"
#include "debug.h"
#include "vector.h"
#include "panels.h"

static char load_buf[CONF_LOAD_BUF_MAX];

int cull_backface;

void panels_init(void)
{
	memset(load_buf, 0, CONF_LOAD_BUF_MAX);
}

static void _panel_props_load_and_export(struct nk_context *ctx,
					 object_t **obj_cur)
{
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, load_buf,
				CONF_LOAD_BUF_MAX, nk_filter_default);
	/* TODO: Make sure to add instancing at some point */
	nk_layout_row_dynamic(ctx, 30, 3);
	if (nk_button_label(ctx, "Load GLB Object"))
	{
		char full_load_buf[CONF_LOAD_BUF_MAX + 11];

		sprintf(full_load_buf, "res/models/%s", load_buf);
		scene_object_add(scene, full_load_buf);
		*obj_cur = scene->objects[scene->num_objects - 1];
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
		debugf("Loaded GLB Object '%s' from '%s'\n",
	 scene->objects[scene->num_objects - 1], full_load_buf);
	}

	if (nk_button_label(ctx, "Load Scene"))
	{
		free(scene);
		scene = scene_create_file(load_buf);
		*obj_cur = scene->objects[0];
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	}

	if (nk_button_label(ctx, "Save Scene"))
	{
		scene_write_file(scene, load_buf);
		memset(load_buf, 0, CONF_LOAD_BUF_MAX);
	}
}

static void _panel_props_object(struct nk_context *ctx, object_t *obj_cur)
{
	if (!obj_cur)
		return;

	nk_layout_row_dynamic(ctx, 30, 4);
	nk_label(ctx, "Position", NK_TEXT_LEFT);
	nk_property_float(ctx, "X", -INFINITY, &obj_cur->trans[3][0],
		   INFINITY, 0.001f, 0.001f);
	nk_property_float(ctx, "Y", -INFINITY, &obj_cur->trans[3][1],
		   INFINITY, 0.001f, 0.001f);
	nk_property_float(ctx, "Z", -INFINITY, &obj_cur->trans[3][2],
		   INFINITY, 0.001f, 0.001f);
	nk_layout_row_dynamic(ctx, 30, 1);
	if (nk_button_label(ctx, "Reset Position"))
	{
		memset(obj_cur->trans[3], 0, sizeof(float) * 3);
		debugf("Reset Position of '%s'\n", obj_cur->name);
	}

	const char *obj_name = obj_cur->name;
	const uint32_t obj_flags = obj_cur->flags;
	uint32_t *obj_flagsp = &obj_cur->flags;

	nk_layout_row_dynamic(ctx, 30, 3);
	if (nk_checkbox_flags_label(ctx, "Has Collision",
			     obj_flagsp, OBJ_HAS_COLLISION))
		debugf("Toggled %s's collission flag %s\n",
	 obj_name, (obj_flags & OBJ_HAS_COLLISION) ? "on" : "off");

	if (nk_checkbox_flags_label(ctx, "Is Visible",
			     obj_flagsp, OBJ_IS_VISIBLE))
		debugf("Toggled %s's visibility flag %s\n",
	 obj_name, (obj_flags & OBJ_IS_VISIBLE) ? "on" : "off");

	if (nk_checkbox_flags_label(ctx, "Is Pickup",
			     obj_flagsp, OBJ_IS_PICKUP))
		debugf("Toggled %s's pickup flag %s\n",
	 obj_name, (obj_flags & OBJ_IS_PICKUP) ? "on" : "off");
}

void panel_props(void *ctxin, object_t **obj_cur)
{
	struct nk_context *ctx = ctxin;
	static const int width = 420;

	if (!nk_begin(ctx, *obj_cur ? (*obj_cur)->name : "null",
	      nk_rect(0, 0, width, CONF_HEIGHT),
	      NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);
	_panel_props_load_and_export(ctx, obj_cur);
	_panel_props_object(ctx, *obj_cur);

	nk_layout_row_dynamic(ctx, 30, 1);
	nk_label(ctx, "Global", NK_TEXT_LEFT);
	nk_checkbox_label(ctx, "Backface Culling", &cull_backface);
	nk_end(ctx);
}

void panel_list(void *ctxin, object_t **obj_cur)
{
	struct nk_context *ctx = ctxin;
	static const int width = 240;

	if (!nk_begin(ctx, "Object List", nk_rect(CONF_WIDTH - width, 0, width,
					(CONF_HEIGHT >> 1) * 1.5f),
	      NK_WINDOW_TITLE | NK_WINDOW_BORDER))
		return;

	nk_layout_row_dynamic(ctx, 30, 1);
	for (int i = 0; i < scene->num_objects; i++)
	{
		object_t *obj = scene->objects[i];

		if (nk_option_label(ctx, obj->name, obj == *obj_cur))
			*obj_cur = obj;
	}

	nk_end(ctx);
}
