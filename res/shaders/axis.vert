#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 2) in vec3 a_norm;

uniform mat4 u_proj;
uniform mat4 u_view;

out vec3 o_norm;

void main()
{
	o_norm = a_norm;
	gl_Position = u_proj * u_view * vec4(a_pos, 1.0);
}
