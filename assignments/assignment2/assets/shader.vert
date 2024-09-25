#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 fColor;
out vec2 TexCoord;

//uniform float waveT;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	fColor = aColor;
	TexCoord = aTexCoord;
}
