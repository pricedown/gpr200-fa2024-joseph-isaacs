#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 fColor;

uniform float waveT;

void main() {
	gl_Position = vec4(cos(waveT*2+aPos.y)*cos(waveT*2+aPos.y)*cos(waveT*2+aPos.y)/1.5f * aPos.x, aPos.y + sin(waveT+(aPos.x))/2.0f, aPos.z, 1.0f);
	fColor = aColor;
}
