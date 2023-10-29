#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_norm;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 o_uv;

void main()
{
	o_uv = a_uv;
	gl_Position = u_proj * u_view * u_model * vec4(a_pos, 1.0);
}
