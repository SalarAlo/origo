#VERTEX

#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUv;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform float u_Time;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vUv;

void main() {
    vec3 worldPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));

    float waveFreq  = 1.5;
    float waveAmp   = 0.35;
    float waveSpeed = 1.0;

    float dist = length(worldPos.xz);
    worldPos.y += sin(dist * waveFreq - u_Time * waveSpeed) * waveAmp;

    mat3 normalMatrix = mat3(transpose(inverse(u_ModelMatrix)));
    vNormal = normalize(normalMatrix * aNorm);
    vFragPos = worldPos;
    vUv = aUv;

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(worldPos, 1.0);
}

#FRAGMENT

#version 430 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vUv;

out vec4 FragColor;

uniform vec3 u_ViewPos;

void main() {
    vec3 lightPos = vec3(1.0, 8.0, 1.0);
    vec3 lightColor = vec3(1.0);

    vec3 baseColor = vec3(0.4, 0.7, 1.0);

    vec3 ambient = 0.2 * lightColor;

    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(u_ViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * baseColor;
    FragColor = vec4(result, 1.0);
}
