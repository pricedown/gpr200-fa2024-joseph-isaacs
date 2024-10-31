#version 330 core

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  

out vec4 FragColor;

uniform sampler2D tex;
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;  

void main() {
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * vec3(texture(tex,TexCoord));
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(Normal,1.0);
}