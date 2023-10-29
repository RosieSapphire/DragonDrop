#include <stdio.h>
#include <malloc.h>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>

#include "vector.h"
#include "mat4.h"
#include "object.h"

object_t *object_selected = NULL;

object_t *object_create_empty(void)
{
	object_t *obj = malloc(sizeof(object_t));
	obj->flags = 0;
	obj->mesh = NULL;
	mat4_identity(obj->trans);
	strncpy(obj->name, "Nothing", CONF_NAME_MAX);
	object_selected = obj;
	return obj;
}

object_t *object_create_tri(void)
{
	printf("Generating Triangle...\n");

	object_t *obj = malloc(sizeof(object_t));
	strncpy(obj->name, "Triangle", CONF_NAME_MAX);
	obj->mesh = mesh_create_tri();
	obj->aabb = aabb_from_mesh(obj->mesh);
	mat4_identity(obj->trans);
	obj->flags = OBJ_HAS_COLLISION | OBJ_IS_VISIBLE;
	object_selected = obj;

	return obj;
}

object_t *object_create_file(const char *path)
{
	const int flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials | aiProcess_FlipWindingOrder;
	const struct aiScene *scene = aiImportFile(path, flags);
	object_t *obj = NULL;
	if(!scene) {
		fprintf(stderr, "Failed to open object scene '%s'\n", path);
		return NULL;
	}

	if(!scene->mNumMeshes) {
		fprintf(stderr, "'%s' does not have any meshes.\n", path);
		return NULL;
	}

	printf("Opened object scene '%s'\n", path);

	obj = malloc(sizeof(object_t));
	obj->flags = OBJ_HAS_COLLISION | OBJ_IS_VISIBLE;

	const struct aiMesh *aimesh = scene->mMeshes[0];
	strncpy(obj->name, aimesh->mName.data, CONF_NAME_MAX);

	const uint16_t num_verts = aimesh->mNumVertices;
	vertex_t *verts = malloc(sizeof(vertex_t) * num_verts);
	for(int i = 0; i < num_verts; i++) {
		vertex_t *vert = verts + i;
		vert->pos[0] = aimesh->mVertices[i].x;
		vert->pos[1] = aimesh->mVertices[i].y;
		vert->pos[2] = aimesh->mVertices[i].z;
		vert->uv[0] = aimesh->mTextureCoords[0][i].x;
		vert->uv[1] = aimesh->mTextureCoords[0][i].y;
		vert->norm[0] = aimesh->mNormals[i].x;
		vert->norm[1] = aimesh->mNormals[i].y;
		vert->norm[2] = aimesh->mNormals[i].z;
	}

	const uint16_t num_indis = aimesh->mNumFaces * 3;
	uint16_t *indis = malloc(sizeof(uint16_t) * num_indis);
	for(int i = 0; i < num_indis / 3; i++) {
		for(int j = 0; j < 3; j++) {
			uint16_t *indi = indis + (3 * i) + j;
			*indi = aimesh->mFaces[i].mIndices[j];
		}
	}

	obj->mesh = mesh_create_data(num_verts, num_indis, verts, indis);
	obj->aabb = aabb_from_mesh(obj->mesh);
	mat4_identity(obj->trans);
	object_selected = obj;

	free(verts);
	free(indis);

	return obj;
}

void object_draw(const object_t *obj, const uint32_t shader,
		 float proj_mat[4][4], float view_mat[4][4], bool is_selected)
{
	if(!(obj->flags & OBJ_IS_VISIBLE))
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
	mesh_draw(obj->mesh);
}

void object_destroy(object_t *o)
{
	printf("Destroying Triangle...\n");
	o->flags = 0;
	mesh_destroy(o->mesh);
	free(o);
}
