#include <stdio.h>
#include <malloc.h>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "object.h"

object_t *object_gen_tri(void)
{
	object_t *obj = malloc(sizeof(object_t));
	obj->flags = OBJ_IS_ACTIVE | OBJ_IS_VISIBLE;
	printf("Generating Triangle...\n");
	obj->mesh = mesh_create_tri();

	return obj;
}

/*
object_t *object_load_file(const char *path)
{
	const int flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes |
		aiProcess_GenBoundingBoxes | aiProcess_ImproveCacheLocality |
		aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials;
	const struct aiScene *scene = aiImportFile(path, flags);
}
*/

void object_destroy(object_t *o)
{
	printf("Destroying Triangle...\n");
	o->flags = 0;
	mesh_destroy(o->mesh);
	free(o);
}
