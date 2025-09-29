#version 330 core
//--- in_Position: attribute index 0 
//--- in_Color: attribute index 1

layout (location = 0) in vec3 in_Position; 
layout (location = 1) in vec3 in_Color; 
out vec3 out_color;

void main()
{
	gl_Position = vec4(in_Position, 1.0);

	out_color = in_Color;
}