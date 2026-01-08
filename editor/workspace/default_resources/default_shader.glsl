#VERTEX

#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUv;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vUv;

out float v_Depth;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(u_ModelMatrix)));

    vNormal = normalize(normalMatrix * aNorm);
    vFragPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));
    vUv = aUv;

    vec4 viewPos =  u_ViewMatrix * vec4(vFragPos, 1.0);
    v_Depth = -viewPos.z;

    gl_Position = u_ProjectionMatrix * viewPos;
}

#FRAGMENT

#version 430 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vUv;

out vec4 FragColor;

uniform vec3 u_ViewPos;
uniform sampler2D u_Texture_Albedo;

uniform float u_Ambient = 0.2;
uniform int u_ShinyFactor = 32;
uniform vec3 u_LightColor = vec3(1.0);

uniform vec3 u_LightPos = vec3(1.0, 8.0, 1.0);

void main() {
    vec3 objectColor = texture(u_Texture_Albedo, vUv).rgb;

    vec3 lightDir = normalize(u_LightPos - vFragPos);
    vec3 norm = normalize(vNormal);

    vec3 viewDir = normalize(u_ViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambientColor = u_Ambient * u_LightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_ShinyFactor);
    vec3 specular = 0.5 * spec * u_LightColor;

    vec3 litColor = (ambientColor + diffuse + specular) * objectColor;
    FragColor = vec4(litColor, 1.0);

}
