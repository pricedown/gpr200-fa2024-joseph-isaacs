#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 fColor;
out vec2 TexCoord;

uniform float waveT;

void main() {
	gl_Position = vec4(0.66f + aPos.x + cos(waveT)/2, 0.66f + aPos.y + sin(waveT)/2, aPos.z, 1.0f);
	fColor = aColor;
	TexCoord = aTexCoord;
}
