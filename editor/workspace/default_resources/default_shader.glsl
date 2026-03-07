#VERTEX

#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_norm;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec4 a_tangent;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_frag_pos;
out vec2 v_uv;

void main() {
        mat3 normal_matrix = mat3(transpose(inverse(u_model_matrix)));

        v_normal = normalize(normal_matrix * a_norm);
        vec3 tangent_ws = normal_matrix * a_tangent.xyz;
        if (length(tangent_ws) < 0.0001) {
                vec3 up = abs(v_normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
                tangent_ws = cross(up, v_normal);
        }
        v_tangent = normalize(tangent_ws - dot(tangent_ws, v_normal) * v_normal);
        v_bitangent = normalize(cross(v_normal, v_tangent) * a_tangent.w);
        v_frag_pos = vec3(u_model_matrix * vec4(a_pos, 1.0));
        v_uv = a_uv;

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
in vec3 v_tangent;
in vec3 v_bitangent;
in vec3 v_frag_pos;
in vec2 v_uv;

out vec4 frag_color;

uniform vec3 u_view_pos;

uniform vec3 u_base_color_factor = vec3(1.0);
uniform float u_metallic_factor = 0.0;
uniform float u_roughness_factor = 1.0;
uniform float u_ao_factor = 1.0;
uniform bool u_unlit = false;
uniform bool u_has_normal_map = false;

uniform float u_ambient = 0.03;

uniform bool u_has_albedo_map = false;
uniform bool u_has_metallic_roughness_map = false;
uniform bool u_has_ao_map = false;
uniform bool u_has_emissive_map = false;

uniform sampler2D u_texture_albedo;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_metallic_roughness;
uniform sampler2D u_texture_ao;
uniform sampler2D u_texture_emissive;

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
        if (u_has_normal_map) {
                vec3 n_tangent = texture(u_texture_normal, v_uv).rgb * 2.0 - 1.0;
                mat3 TBN = mat3(normalize(v_tangent), normalize(v_bitangent), N);
                N = normalize(TBN * n_tangent);
        }
        vec3 V = normalize(u_view_pos - v_frag_pos);

        vec3 albedo = u_base_color_factor;
        if (u_has_albedo_map) {
                vec3 albedo_tex = texture(u_texture_albedo, v_uv).rgb;
                albedo *= pow(albedo_tex, vec3(2.2));
        }

        float metallic = clamp(u_metallic_factor, 0.0, 1.0);
        float roughness = clamp(u_roughness_factor, 0.04, 1.0);
        if (u_has_metallic_roughness_map) {
                vec3 mr = texture(u_texture_metallic_roughness, v_uv).rgb;
                roughness = clamp(roughness * mr.g, 0.04, 1.0);
                metallic = clamp(metallic * mr.b, 0.0, 1.0);
        }

        float ao = clamp(u_ao_factor, 0.0, 1.0);
        if (u_has_ao_map) {
                ao *= texture(u_texture_ao, v_uv).r;
        }

        vec3 emissive = vec3(0.0);
        if (u_has_emissive_map) {
                emissive = pow(texture(u_texture_emissive, v_uv).rgb, vec3(2.2));
        }

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

        vec3 ambient = u_unlit ? albedo : (u_ambient * albedo * ao);
        vec3 color = ambient + Lo + emissive;

        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0 / 2.2));

        frag_color = vec4(color, 1.0);
}
