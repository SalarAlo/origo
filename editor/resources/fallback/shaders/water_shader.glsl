#VERTEX

#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_norm;
layout(location = 2) in vec2 a_uv;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

out vec3 v_world_pos;
out vec3 v_world_normal;
out vec2 v_uv;

void main() {
        mat3 normal_matrix = mat3(transpose(inverse(u_model_matrix)));
        v_world_pos = vec3(u_model_matrix * vec4(a_pos, 1.0));
        v_world_normal = normalize(normal_matrix * a_norm);
        v_uv = a_uv;
        gl_Position = u_projection_matrix * u_view_matrix * vec4(v_world_pos, 1.0);
}

#FRAGMENT

#version 430 core

const float PI = 3.14159265359;

struct directional_light {
        vec3 direction;
        vec3 color;
        float intensity;
};

in vec3 v_world_pos;
in vec3 v_world_normal;
in vec2 v_uv;

out vec4 frag_color;
layout(location = 1) out int entity_id_out;

uniform vec3 u_view_pos;
uniform int u_entity_id = -1;
uniform float u_time = 0.0;
uniform float u_wave_tile_scale = 0.08;
uniform float u_wave_strength = 0.4;
uniform float u_smoothness = 0.92;
uniform float u_ambient = 0.2;

uniform directional_light u_dir_light;

uniform sampler2D u_wave_normal_a;
uniform sampler2D u_wave_normal_b;

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
        return geometry_schlick_ggx(NdotV, roughness) * geometry_schlick_ggx(NdotL, roughness);
}

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
        return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 sample_wave_normal(vec2 uv_a, vec2 uv_b, float strength) {
        vec3 na = texture(u_wave_normal_a, uv_a).xyz * 2.0 - 1.0;
        vec3 nb = texture(u_wave_normal_b, uv_b).xyz * 2.0 - 1.0;
        vec3 tangent_normal = normalize(mix(vec3(0.0, 0.0, 1.0), normalize(na + nb), strength));
        vec3 tangent = normalize(vec3(1.0, 0.0, 0.0));
        vec3 bitangent = normalize(vec3(0.0, 0.0, 1.0));
        vec3 normal = normalize(v_world_normal);
        return normalize(tangent * tangent_normal.x + bitangent * tangent_normal.y + normal * tangent_normal.z);
}

void main() {
        vec2 uv_a = v_world_pos.xz * u_wave_tile_scale + vec2(u_time * 0.04, u_time * 0.02);
        vec2 uv_b = v_world_pos.xz * (u_wave_tile_scale * 1.6) + vec2(-u_time * 0.03, u_time * 0.05);
        vec3 N = sample_wave_normal(uv_a, uv_b, u_wave_strength);
        vec3 V = normalize(u_view_pos - v_world_pos);
        vec3 L = normalize(-u_dir_light.direction);
        vec3 H = normalize(V + L);

        vec3 shallow = vec3(0.08, 0.33, 0.42);
        vec3 deep = vec3(0.02, 0.11, 0.18);
        float view_fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
        vec3 albedo = mix(shallow, deep, 0.55 + 0.35 * (1.0 - max(N.y, 0.0)));
        float roughness = clamp(1.0 - u_smoothness, 0.04, 0.35);

        float NDF = distribution_ggx(N, H, roughness);
        float G = geometry_smith(N, V, L, roughness);
        vec3 F = fresnel_schlick(max(dot(H, V), 0.0), vec3(0.02));
        vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);
        vec3 diffuse = albedo / PI;
        vec3 radiance = u_dir_light.color * u_dir_light.intensity;

        vec3 sky_reflection = mix(vec3(0.18, 0.22, 0.28), vec3(0.52, 0.62, 0.72), view_fresnel);
        vec3 color = (diffuse + specular) * radiance * max(dot(N, L), 0.0);
        color += albedo * u_ambient * 0.55;
        color = mix(color, sky_reflection, view_fresnel * 0.65);

        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0 / 2.2));
        frag_color = vec4(color, 1.0);
        entity_id_out = u_entity_id;
}
