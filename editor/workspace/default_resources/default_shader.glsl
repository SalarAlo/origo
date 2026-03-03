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

        vec4 viewPos = u_ViewMatrix * vec4(vFragPos, 1.0);
        v_Depth = -viewPos.z;

        gl_Position = u_ProjectionMatrix * viewPos;
}

#FRAGMENT

#version 430 core

struct DirectionalLight {
        vec3 direction;
        vec3 color;
        float intensity;
};

struct PointLight {
        vec3 position;
        vec3 color;
        float intensity;

        float constant;
        float linear;
        float quadratic;
};

struct SpotLight {
        vec3 position;
        vec3 direction;
        vec3 color;
        float intensity;

        float innerCutoff;
        float outerCutoff;

        float constant;
        float linear;
        float quadratic;
};

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vUv;

out vec4 FragColor;

uniform vec3 u_Color = vec3(1.0);
uniform bool u_UseTexture = true;
uniform bool u_UseLight = true;

uniform vec3 u_ViewPos;
uniform sampler2D u_Texture_Albedo;

uniform float u_Ambient = 0.2;
uniform int u_ShinyFactor = 32;

#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS  8

uniform DirectionalLight u_DirLight;

uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];

uniform int u_SpotLightCount;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];

vec3 ApplyBlinnPhong(
        vec3 lightDir,
        vec3 lightColor,
        float intensity,
        vec3 normal,
        vec3 viewDir
) {
        float diff = max(dot(normal, lightDir), 0.0);

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_ShinyFactor);

        vec3 diffuse = diff * lightColor;
        vec3 specular = spec * lightColor * 0.5;

        return intensity * (diffuse + specular);
}

vec3 ComputeDirectionalLight(
        DirectionalLight light,
        vec3 normal,
        vec3 viewDir
) {
        vec3 lightDir = normalize(-light.direction);
        return ApplyBlinnPhong(lightDir, light.color, light.intensity, normal, viewDir);
}

vec3 ComputePointLight(
        PointLight light,
        vec3 fragPos,
        vec3 normal,
        vec3 viewDir
) {
        vec3 lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);

        float attenuation =
                1.0 / (light.constant +
                                light.linear * distance +
                                light.quadratic * distance * distance);

        vec3 result = ApplyBlinnPhong(lightDir, light.color, light.intensity, normal, viewDir);
        return result * attenuation;
}

vec3 ComputeSpotLight(
        SpotLight light,
        vec3 fragPos,
        vec3 normal,
        vec3 viewDir
) {
        vec3 lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);

        float attenuation =
                1.0 / (light.constant +
                                light.linear * distance +
                                light.quadratic * distance * distance);

        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.innerCutoff - light.outerCutoff;
        float coneIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

        vec3 result = ApplyBlinnPhong(lightDir, light.color, light.intensity, normal, viewDir);
        return result * attenuation * coneIntensity;
}

void main()
{
        vec3 albedo = u_Color;

        if (u_UseTexture) {
                vec3 tex = pow(texture(u_Texture_Albedo, vUv).rgb, vec3(2.2));
                albedo *= tex;
        }

        vec3 normal = normalize(vNormal);
        vec3 viewDir = normalize(u_ViewPos - vFragPos);

        vec3 lighting = albedo;
        if (u_UseLight) {
                lighting *= u_Ambient;

                lighting += ComputeDirectionalLight(
                                u_DirLight,
                                normal,
                                viewDir
                        ) * albedo;

                for (int i = 0; i < u_PointLightCount; ++i)
                {
                        lighting += ComputePointLight(
                                        u_PointLights[i],
                                        vFragPos,
                                        normal,
                                        viewDir
                                ) * albedo;
                }

                for (int i = 0; i < u_SpotLightCount; ++i)
                {
                        lighting += ComputeSpotLight(
                                        u_SpotLights[i],
                                        vFragPos,
                                        normal,
                                        viewDir
                                ) * albedo;
                }
        }

        FragColor = vec4(pow(lighting, vec3(1.0 / 2.2)), 1.0);
}
