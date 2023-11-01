#include <stdio.h>
#include <malloc.h>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>

#include "vector.h"
#include "mat4.h"
#include "debug.h"
#include "object.h"

static const int aiflags = aiProcess_Triangulate | aiProcess_OptimizeMeshes |
	aiProcess_ImproveCacheLocality | aiProcess_JoinIdenticalVertices |
	aiProcess_RemoveRedundantMaterials | aiProcess_FlipWindingOrder;

static void _object_load_file_verts(const uint16_t num_verts, vertex_t *verts,
				    const struct aiMesh *aimesh)
{
	for (int i = 0; i < num_verts; i++)
	{
		vertex_t *vert = verts + i;

		vector_copy(vert->pos, (float *)(aimesh->mVertices + i), 3);
		vector_copy(vert->uv,
	      (float *)(aimesh->mTextureCoords[0] + i), 2);
		vector_copy(vert->norm, (float *)(aimesh->mNormals + i), 3);
	}
}

static void _object_load_file_indis(const uint16_t num_indis, uint16_t *indis,
				    const struct aiMesh *aimesh)
{
	for (int i = 0; i < num_indis / 3; i++)
		for (int j = 0; j < 3; j++)
			indis[3 * i + j] = aimesh->mFaces[i].mIndices[j];
}

object_t *object_load_file(const char *path)
{
	const struct aiScene *scene = aiImportFile(path, aiflags);
	object_t *obj = NULL;

	if (!scene)
	{
		debugf("Failed to Load Object '%s'\n", path);
		return (NULL);
	}

	if (!scene->mNumMeshes)
	{
		debugf("Object '%s' has No Meshes.\n", path);
		return (NULL);
	}

	debugf("Loaded Object '%s'\n", path);

	obj = malloc(sizeof(object_t));
	obj->flags = OBJ_HAS_COLLISION | OBJ_IS_VISIBLE;

	const struct aiMesh *aimesh = scene->mMeshes[0];
	const uint16_t num_verts = aimesh->mNumVertices;
	const uint16_t num_indis = aimesh->mNumFaces * 3;
	vertex_t *verts = malloc(sizeof(vertex_t) * num_verts);
	uint16_t *indis = malloc(sizeof(uint16_t) * num_indis);

	strncpy(obj->name, aimesh->mName.data, CONF_NAME_MAX);
	_object_load_file_verts(num_verts, verts, aimesh);
	_object_load_file_indis(num_indis, indis, aimesh);

	obj->mesh = mesh_create_data(num_verts, num_indis, verts, indis);
	obj->aabb = aabb_from_mesh(obj->mesh);
	mat4_identity(obj->trans);
	free(verts);
	free(indis);

	return (obj);
}

void object_draw(const object_t *obj, const uint32_t shader,
		 float proj_mat[4][4], float view_mat[4][4],
		 bool is_selected, bool cull_backface)
{
	if (!(obj->flags & OBJ_IS_VISIBLE))
		return;

	glUseProgram(shader);
	const uint32_t proj_loc = glGetUniformLocation(shader, "u_proj");
	const uint32_t view_loc = glGetUniformLocation(shader, "u_view");
	const uint32_t model_loc = glGetUniformLocation(shader, "u_model");
	const uint32_t is_selected_loc =
		glGetUniformLocation(shader, "u_is_selected");
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (const float *)proj_mat);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (const float *)view_mat);
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (const float *)obj->trans);
	glUniform1i(is_selected_loc, is_selected);
	mesh_draw(obj->mesh, cull_backface);
}

void object_destroy(object_t *o)
{
	debugf("Destroying Triangle\n");
	o->flags = 0;
	mesh_destroy(o->mesh);
	free(o);
}
