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

struct point_light {
        vec3 position;
        vec3 color;
        float intensity;
        float constant;
        float linear;
        float quadratic;
};

in vec3 v_world_pos;
in vec3 v_world_normal;
in vec2 v_uv;

out vec4 frag_color;
layout(location = 1) out int entity_id_out;

uniform vec3 u_view_pos;
uniform int u_entity_id = -1;
uniform float u_ambient = 0.2;

uniform vec3 u_dir_light_direction = vec3(0.4, -1.0, 0.3);
uniform vec3 u_dir_light_color = vec3(1.0);
uniform float u_dir_light_intensity = 1.0;

#define max_point_lights 8
uniform directional_light u_dir_light;
uniform int u_point_light_count;
uniform point_light u_point_lights[max_point_lights];

uniform float u_terrain_max_height = 50.0;
uniform float u_water_level = 0.18;
uniform bool u_use_texture_layers = false;

uniform float u_ground_tile_scale = 0.08;
uniform float u_rock_tile_scale = 0.06;
uniform float u_sand_tile_scale = 0.11;
uniform float u_snow_tile_scale = 0.07;
uniform float u_micro_detail_scale = 0.24;
uniform float u_micro_detail_strength = 0.28;

uniform float u_rock_slope_start = 0.18;
uniform float u_rock_slope_end = 0.46;
uniform float u_cliff_slope_start = 0.32;
uniform float u_cliff_slope_end = 0.68;
uniform float u_shoreline_width = 0.055;
uniform float u_wetness_width = 0.04;
uniform float u_snow_start = 0.78;
uniform float u_snow_end = 0.93;
uniform float u_snow_slope_limit = 0.24;
uniform float u_height_blend_sharpness = 0.35;

uniform sampler2D u_ground_albedo;
uniform sampler2D u_ground_normal;
uniform sampler2D u_ground_packed;
uniform sampler2D u_rock_albedo;
uniform sampler2D u_rock_normal;
uniform sampler2D u_rock_packed;
uniform sampler2D u_sand_albedo;
uniform sampler2D u_sand_normal;
uniform sampler2D u_sand_packed;
uniform sampler2D u_snow_albedo;
uniform sampler2D u_snow_normal;
uniform sampler2D u_snow_packed;
uniform sampler2D u_micro_normal;

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

vec3 hash33(vec3 p) {
        p = vec3(dot(p, vec3(127.1, 311.7, 74.7)), dot(p, vec3(269.5, 183.3, 246.1)), dot(p, vec3(113.5, 271.9, 124.6)));
        return fract(sin(p) * 43758.5453);
}

float terrain_noise(vec2 p) {
        vec2 i = floor(p);
        vec2 f = fract(p);
        vec2 u = f * f * (3.0 - 2.0 * f);
        float a = hash33(vec3(i, 1.0)).x;
        float b = hash33(vec3(i + vec2(1.0, 0.0), 1.0)).x;
        float c = hash33(vec3(i + vec2(0.0, 1.0), 1.0)).x;
        float d = hash33(vec3(i + vec2(1.0, 1.0), 1.0)).x;
        return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

vec3 triplanar_weights(vec3 n) {
        vec3 blend = pow(abs(n), vec3(4.0));
        return blend / max(dot(blend, vec3(1.0)), 0.0001);
}

vec4 sample_triplanar(sampler2D tex, vec3 pos, vec3 blend, float scale) {
        vec4 x = texture(tex, pos.zy * scale);
        vec4 y = texture(tex, pos.xz * scale);
        vec4 z = texture(tex, pos.xy * scale);
        return x * blend.x + y * blend.y + z * blend.z;
}

vec3 tangent_from_normal(vec3 n) {
        vec3 up = abs(n.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        return normalize(cross(up, n));
}

vec3 sample_planar_detail_normal(sampler2D tex, vec2 uv, vec3 base_normal, float strength) {
        vec3 tangent_normal = texture(tex, uv).xyz * 2.0 - 1.0;
        tangent_normal = normalize(mix(vec3(0.0, 0.0, 1.0), tangent_normal, strength));
        vec3 T = tangent_from_normal(base_normal);
        vec3 B = normalize(cross(base_normal, T));
        return normalize(T * tangent_normal.x + B * tangent_normal.y + base_normal * tangent_normal.z);
}

vec3 evaluate_pbr(vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness) {
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

vec3 evaluate_soft_directional(vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness) {
        vec3 H = normalize(V + L);
        float NDF = distribution_ggx(N, H, roughness);
        float G = geometry_smith(N, V, L, roughness);
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F = fresnel_schlick(max(dot(H, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float wrapped_NdotL = clamp((NdotL + 0.28) / 1.28, 0.0, 1.0);
        vec3 specular = (NDF * G * F) / max(4.0 * NdotV * max(NdotL, 0.001), 0.0001);
        vec3 diffuse = kD * albedo / PI;
        return (diffuse * wrapped_NdotL + specular * mix(0.65, 1.0, NdotL)) * radiance;
}

void main() {
        vec3 base_normal = normalize(v_world_normal);
        vec3 blend = triplanar_weights(base_normal);
        float normalized_height = clamp(v_world_pos.y / max(u_terrain_max_height, 0.0001), 0.0, 1.0);
        float slope = 1.0 - clamp(base_normal.y, 0.0, 1.0);

        float large_variation = terrain_noise(v_world_pos.xz * 0.012);
        float fine_variation = terrain_noise(v_world_pos.xz * 0.065 + vec2(13.7, 4.9));

        float rock_mask = smoothstep(u_rock_slope_start, u_rock_slope_end, slope);
        float cliff_mask = smoothstep(u_cliff_slope_start, u_cliff_slope_end, slope);

        float shore_distance = abs(normalized_height - u_water_level);
        float sand_mask = (1.0 - smoothstep(0.0, u_shoreline_width, shore_distance)) * (1.0 - cliff_mask);
        float wet_mask = (1.0 - smoothstep(0.0, u_wetness_width, max(normalized_height - u_water_level, 0.0))) * (1.0 - cliff_mask);
        float snow_mask = smoothstep(u_snow_start, u_snow_end, normalized_height) * (1.0 - smoothstep(u_snow_slope_limit, u_snow_slope_limit + 0.18, slope));
        vec3 albedo;
        vec3 material_normal = base_normal;
        float roughness;

        if (u_use_texture_layers) {
                vec4 ground_albedo_sample = sample_triplanar(u_ground_albedo, v_world_pos, blend, u_ground_tile_scale);
                vec4 rock_albedo_sample = sample_triplanar(u_rock_albedo, v_world_pos, blend, u_rock_tile_scale);
                vec4 sand_albedo_sample = sample_triplanar(u_sand_albedo, v_world_pos, blend, u_sand_tile_scale);
                vec4 snow_albedo_sample = sample_triplanar(u_snow_albedo, v_world_pos, blend, u_snow_tile_scale);

                vec4 ground_packed = sample_triplanar(u_ground_packed, v_world_pos, blend, u_ground_tile_scale);
                vec4 rock_packed = sample_triplanar(u_rock_packed, v_world_pos, blend, u_rock_tile_scale);
                vec4 sand_packed = sample_triplanar(u_sand_packed, v_world_pos, blend, u_sand_tile_scale);
                vec4 snow_packed = sample_triplanar(u_snow_packed, v_world_pos, blend, u_snow_tile_scale);

                vec3 ground_normal = sample_planar_detail_normal(u_ground_normal, v_world_pos.xz * u_ground_tile_scale, base_normal, 0.10);
                vec3 rock_normal = sample_planar_detail_normal(u_rock_normal, v_world_pos.xz * u_rock_tile_scale, base_normal, 0.16);
                vec3 sand_normal = sample_planar_detail_normal(u_sand_normal, v_world_pos.xz * u_sand_tile_scale, base_normal, 0.06);
                vec3 snow_normal = sample_planar_detail_normal(u_snow_normal, v_world_pos.xz * u_snow_tile_scale, base_normal, 0.04);
                vec3 micro_normal = sample_planar_detail_normal(u_micro_normal, v_world_pos.xz * u_micro_detail_scale, base_normal, 0.08);

                rock_mask = clamp(rock_mask + (rock_packed.r - ground_packed.r) * u_height_blend_sharpness, 0.0, 1.0);
                cliff_mask = clamp(cliff_mask + (rock_packed.r - ground_packed.r) * 0.25, 0.0, 1.0);
                sand_mask = (1.0 - smoothstep(0.0, u_shoreline_width, shore_distance)) * (1.0 - cliff_mask);
                wet_mask = (1.0 - smoothstep(0.0, u_wetness_width, max(normalized_height - u_water_level, 0.0))) * (1.0 - cliff_mask);
                snow_mask = smoothstep(u_snow_start, u_snow_end, normalized_height) * (1.0 - smoothstep(u_snow_slope_limit, u_snow_slope_limit + 0.18, slope));

                vec3 ground_tint = mix(vec3(0.95, 0.97, 0.94), vec3(1.03, 1.00, 0.96), large_variation);
                vec3 rock_tint = mix(vec3(0.96, 0.97, 0.99), vec3(1.02, 1.00, 0.98), fine_variation);
                vec3 sand_tint = mix(vec3(0.96, 0.95, 0.92), vec3(1.02, 1.00, 0.96), large_variation);
                vec3 snow_tint = mix(vec3(0.98, 0.99, 1.00), vec3(1.01, 1.00, 0.99), fine_variation);

                albedo = ground_albedo_sample.rgb * ground_tint;
                material_normal = ground_normal;
                roughness = ground_packed.g;

                albedo = mix(albedo, rock_albedo_sample.rgb * rock_tint, rock_mask);
                material_normal = normalize(mix(material_normal, rock_normal, rock_mask));
                roughness = mix(roughness, rock_packed.g, rock_mask);

                albedo = mix(albedo, sand_albedo_sample.rgb * sand_tint, sand_mask);
                material_normal = normalize(mix(material_normal, sand_normal, sand_mask));
                roughness = mix(roughness, sand_packed.g, sand_mask);

                albedo = mix(albedo, snow_albedo_sample.rgb * snow_tint, snow_mask);
                material_normal = normalize(mix(material_normal, snow_normal, snow_mask));
                roughness = mix(roughness, snow_packed.g, snow_mask);

                material_normal = normalize(mix(material_normal, micro_normal, u_micro_detail_strength * 0.35));
                albedo *= mix(1.0, 0.92, wet_mask);
                roughness = mix(roughness, 0.82, wet_mask * 0.45);
                albedo = clamp(albedo, vec3(0.0), vec3(0.78));
        } else {
                vec3 ground_color = mix(vec3(0.18, 0.34, 0.16), vec3(0.32, 0.39, 0.20), large_variation);
                vec3 rock_color = mix(vec3(0.33, 0.34, 0.36), vec3(0.43, 0.41, 0.39), fine_variation);
                vec3 sand_color = mix(vec3(0.56, 0.50, 0.37), vec3(0.69, 0.64, 0.52), large_variation);
                vec3 snow_color = mix(vec3(0.88, 0.90, 0.94), vec3(0.97, 0.98, 1.0), fine_variation);

                albedo = ground_color;
                albedo = mix(albedo, rock_color, rock_mask);
                albedo = mix(albedo, sand_color, sand_mask);
                albedo = mix(albedo, snow_color, snow_mask);
                albedo *= mix(1.0, 0.90, wet_mask);
                roughness = mix(0.88, 0.62, rock_mask);
                roughness = mix(roughness, 0.76, sand_mask);
                roughness = mix(roughness, 0.44, snow_mask);
        }

        vec3 N = normalize(material_normal);
        vec3 V = normalize(u_view_pos - v_world_pos);
        vec3 Lo = vec3(0.0);
        float metallic = 0.02;

        vec3 dir_L = normalize(-u_dir_light.direction);
        vec3 dir_radiance = u_dir_light.color * u_dir_light.intensity;
        Lo += evaluate_soft_directional(dir_radiance, N, V, dir_L, albedo, metallic, clamp(roughness, 0.05, 1.0));

        for (int i = 0; i < u_point_light_count; ++i) {
                point_light light = u_point_lights[i];
                vec3 L = light.position - v_world_pos;
                float distance = length(L);
                L = normalize(L);
                float attenuation = 1.0 / max(light.constant + light.linear * distance + light.quadratic * distance * distance, 0.0001);
                vec3 radiance = light.color * light.intensity * attenuation;
                Lo += evaluate_pbr(radiance, N, V, L, albedo, metallic, clamp(roughness, 0.05, 1.0));
        }

        float NdotUp = clamp(N.y * 0.5 + 0.5, 0.0, 1.0);
        float view_fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
        vec3 sky_ambient = mix(vec3(0.24, 0.23, 0.22), vec3(0.46, 0.52, 0.60), NdotUp);
        vec3 ground_bounce = vec3(0.10, 0.085, 0.07) * (1.0 - clamp(N.y, 0.0, 1.0));
        float ao = mix(1.0, 0.88, cliff_mask * 0.25 + rock_mask * 0.10);
        vec3 ambient = (sky_ambient + ground_bounce) * albedo * (0.22 + u_ambient * 0.75) * ao;
        ambient += vec3(0.014) * view_fresnel;

        vec3 color = ambient + Lo;
        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0 / 2.2));

        frag_color = vec4(color, 1.0);
        entity_id_out = u_entity_id;
}
