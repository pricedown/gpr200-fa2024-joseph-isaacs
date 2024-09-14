#version 330 core

out vec4 FragColor;
uniform float alphaShift;
uniform vec3 chromaColor;

in vec3 fColor;


void main()
{
	for (int i = 0; i < 3; i++) {
		FragColor[i] = mod(fColor[i] * chromaColor[i], 1.0f);
	}
} 
