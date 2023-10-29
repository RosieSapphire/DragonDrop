#version 330 core

in vec3 o_norm;

out vec4 frag_color;

void main()
{
	frag_color = vec4(o_norm, 1);
}
