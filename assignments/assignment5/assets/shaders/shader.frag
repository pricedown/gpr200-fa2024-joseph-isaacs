#version 330 core

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  

out vec4 FragColor;

uniform float specularStrength;
uniform float shininess;
uniform bool blinnPhong;

uniform sampler2D tex;
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

void main() {
    vec3 objectColor = vec3(texture(tex,TexCoord));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float spec = 0.0;
    if (blinnPhong)
    {
        spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = lightColor * spec;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(Normal,1.0);
}