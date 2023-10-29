#version 330 core

in vec2 o_uv;

uniform bool u_is_selected;

out vec4 frag_color;

void main()
{
	frag_color = mix(vec4(o_uv, 0.0, 1.0), vec4(1, 1, 0, 1), u_is_selected);
}
