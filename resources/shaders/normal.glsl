#VERTEX

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUv;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vUv;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(u_ModelMatrix)));

    vNormal = normalize(normalMatrix * aNorm);
    vFragPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));
    vUv = aUv;

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(vFragPos, 1.0);
}

#FRAGMENT

#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vUv;

out vec4 FragColor;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform sampler2D u_Texture_Albedo;

void main() {
    vec3 objectColor = texture(u_Texture_Albedo, vUv).rgb;

    // lighting
    vec3 lightColor = vec3(1.0);

    // Ambient
    vec3 ambient = 0.2 * lightColor;

    // Diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(u_LightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(u_ViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
