#ifndef SHADER_H_
#define SHADER_H_

unsigned int shader_create(const char *vpath, const char *fpath);
void shader_destroy(unsigned int s);

#endif /* SHADER_H_ */
