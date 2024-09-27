#version 330 core

out vec4 FragColor;
//uniform float alphaShift;
//uniform vec3 chromaColor;

in vec4 fColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord);
} 


