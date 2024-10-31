#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform float ambientStrength;
uniform vec3 lightColor;

void main() {
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * vec3(texture(tex, TexCoord));
    FragColor = vec4(result, 1.0);
}