#version 330 core
//--- in_Position: attribute index 0 
//--- in_Color: attribute index 1

layout (location = 0) in vec3 vPos; 
layout (location = 1) in vec3 vNormal;

out vec4 FragPos;
out vec3 Normal;

uniform	mat4 model;
uniform	mat4 view;
uniform	mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	FragPos = model * vec4(vPos, 1.0);
	Normal = normalMatrix * vNormal;
	gl_Position = projection * view * FragPos;
}

/*
layout (location = 0) in vec3 in_Position; 
layout (location = 1) in vec3 in_Color; 
out vec3 out_color;

uniform	mat4 model;
uniform	mat4 view;
uniform	mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(in_Position, 1.0);

	out_color = in_Color;
}
*/