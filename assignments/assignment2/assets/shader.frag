#version 330 core

out vec4 FragColor;
//uniform float alphaShift;
//uniform vec3 chromaColor;

in vec4 fColor;

void main()
{
	FragColor = fColor;
} 
