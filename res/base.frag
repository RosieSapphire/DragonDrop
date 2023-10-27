#version 330 core

in vec2 o_uv;

out vec4 frag_color;

void main()
{
	frag_color = vec4(o_uv, 0.0, 1.0);
}
