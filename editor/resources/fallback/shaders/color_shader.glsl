#VERTEX

#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_norm;
layout(location = 2) in vec3 a_color;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

out vec3 v_normal;
out vec3 v_frag_pos;
out vec3 v_color;

void main() {
        mat3 normal_matrix = mat3(transpose(inverse(u_model_matrix)));

        v_normal = normalize(normal_matrix * a_norm);
        v_frag_pos = vec3(u_model_matrix * vec4(a_pos, 1.0));
        v_color = a_color;

        gl_Position = u_projection_matrix * u_view_matrix * vec4(v_frag_pos, 1.0);
}

#FRAGMENT

#version 430 core

const float PI = 3.14159265359;

struct directional_light {
        vec3 direction;
        vec3 color;
        float intensity;
};

struct point_light {
        vec3 position;
        vec3 color;
        float intensity;

        float constant;
        float linear;
        float quadratic;
};

in vec3 v_normal;
in vec3 v_frag_pos;
in vec3 v_color;

out vec4 frag_color;
layout(location = 1) out int entity_id_out;

uniform vec3 u_view_pos;
uniform int u_entity_id = -1;

uniform bool u_unlit = false;
uniform float u_ambient = 0.03;

#define max_point_lights 8

uniform directional_light u_dir_light;

uniform int u_point_light_count;
uniform point_light u_point_lights[max_point_lights];

float distribution_ggx(vec3 N, vec3 H, float roughness) {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = max(dot(N, H), 0.0);
        float NdotH2 = NdotH * NdotH;

        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
        return a2 / max(PI * denom * denom, 0.0000001);
}

float geometry_schlick_ggx(float NdotV, float roughness) {
        float r = roughness + 1.0;
        float k = (r * r) / 8.0;
        return NdotV / max(NdotV * (1.0 - k) + k, 0.0000001);
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float ggxV = geometry_schlick_ggx(NdotV, roughness);
        float ggxL = geometry_schlick_ggx(NdotL, roughness);
        return ggxV * ggxL;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
        return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 evaluate_pbr(
        vec3 radiance,
        vec3 N,
        vec3 V,
        vec3 L,
        vec3 albedo,
        float metallic,
        float roughness
) {
        vec3 H = normalize(V + L);

        float NDF = distribution_ggx(N, H, roughness);
        float G = geometry_smith(N, V, L, roughness);

        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);

        vec3 specular = (NDF * G * F) / max(4.0 * NdotV * NdotL, 0.0001);
        vec3 diffuse = kD * albedo / PI;

        return (diffuse + specular) * radiance * NdotL;
}

void main() {
        vec3 N = normalize(v_normal);
        vec3 V = normalize(u_view_pos - v_frag_pos);
        vec3 albedo = v_color;
        float metallic = 0.0;
        float roughness = 1.0;

        vec3 Lo = vec3(0.0);
        if (!u_unlit) {
                vec3 dir_L = normalize(-u_dir_light.direction);
                vec3 dir_radiance = u_dir_light.color * u_dir_light.intensity;
                Lo += evaluate_pbr(dir_radiance, N, V, dir_L, albedo, metallic, roughness);

                for (int i = 0; i < u_point_light_count; ++i) {
                        point_light light = u_point_lights[i];
                        vec3 L = light.position - v_frag_pos;
                        float distance = length(L);
                        L = normalize(L);

                        float attenuation =
                                1.0 / max(light.constant + light.linear * distance + light.quadratic * distance * distance, 0.0001);

                        vec3 radiance = light.color * light.intensity * attenuation;
                        Lo += evaluate_pbr(radiance, N, V, L, albedo, metallic, roughness);
                }
        }

        vec3 ambient = u_unlit ? albedo : (u_ambient * albedo);
        vec3 color = ambient + Lo;

        frag_color = vec4(color, 1.0);
        entity_id_out = u_entity_id;
}
