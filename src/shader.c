#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <GL/glew.h>

#include "debug.h"
#include "shader.h"

static unsigned int _shader_compile(const char *path, unsigned int type)
{
	FILE *f = fopen(path, "r");
	assert(f);
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	rewind(f);
	char *buf = malloc(fsize);
	fread(buf, sizeof(char), fsize, f);
	buf[fsize - 1] = 0;
	fclose(f);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char **)&buf, NULL);
	glCompileShader(shader);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(!status) {
		char log[512];
		glGetShaderInfoLog(shader, 512, NULL, log);
		fprintf(stderr, "Failed to compile shader from '%s': %s\n",
				path, log);
		exit(1);
	}

	return shader;
}

unsigned int shader_create(const char *vpath, const char *fpath)
{
	unsigned int vshader = _shader_compile(vpath, GL_VERTEX_SHADER);
	unsigned int fshader = _shader_compile(fpath, GL_FRAGMENT_SHADER);
	unsigned int program = glCreateProgram();

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(!status) {
		char log[512];
		glGetProgramInfoLog(program, 512, NULL, log);
		debugf("Failed to link program: %s\n", log);
		exit(1);
	}

	debugf("Loaded Shader (%s, %s)\n", vpath, fpath);

	return program;
}

void shader_destroy(unsigned int s)
{
	glDeleteProgram(s);
}

void shader_use(unsigned int s)
{
	glUseProgram(s);
}
