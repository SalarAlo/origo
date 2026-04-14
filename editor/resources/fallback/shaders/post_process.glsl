#VERTEX

#version 430 core

out vec2 v_uv;

const vec2 verts[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2(3.0, -1.0),
        vec2(-1.0, 3.0)
);

void main() {
        vec2 pos = verts[gl_VertexID];
        gl_Position = vec4(pos, 0.0, 1.0);
        v_uv = pos * 0.5 + 0.5;
}

#FRAGMENT

#version 430 core

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_scene_color;
uniform sampler2D u_bloom_texture;

uniform bool u_post_enabled = true;
uniform bool u_has_bloom = false;
uniform float u_bloom_intensity = 0.0;

uniform bool u_tonemap_enabled = true;
uniform int u_tonemap_operator = 0;
uniform float u_exposure = 0.0;
uniform float u_gamma = 2.2;

uniform bool u_color_adjustments_enabled = false;
uniform float u_contrast = 0.0;
uniform float u_saturation = 1.0;
uniform float u_hue_shift = 0.0;
uniform float u_temperature = 0.0;
uniform float u_tint = 0.0;

uniform bool u_vignette_enabled = false;
uniform float u_vignette_intensity = 0.25;
uniform float u_vignette_smoothness = 0.55;
uniform float u_vignette_roundness = 1.0;
uniform vec3 u_vignette_color = vec3(0.0);

uniform bool u_chromatic_enabled = false;
uniform float u_chromatic_intensity = 0.0015;

uniform bool u_film_grain_enabled = false;
uniform float u_film_grain_intensity = 0.03;
uniform float u_film_grain_response = 0.5;

uniform float u_time = 0.0;
uniform vec2 u_inv_resolution = vec2(0.001);

vec3 sample_scene(vec2 uv) {
        return texture(u_scene_color, clamp(uv, 0.0, 1.0)).rgb;
}

float luminance(vec3 color) {
        return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 tonemap_aces(vec3 x) {
        const float a = 2.51;
        const float b = 0.03;
        const float c = 2.43;
        const float d = 0.59;
        const float e = 0.14;
        return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 tonemap_neutral(vec3 x) {
        return clamp((x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06), 0.0, 1.0);
}

vec3 rgb_to_hsv(vec3 c) {
        vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
        vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
        vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
        float d = q.x - min(q.w, q.y);
        float e = 1.0e-10;
        return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c) {
        vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z * mix(vec3(1.0), rgb, c.y);
}

vec3 apply_color_adjustments(vec3 color) {
        vec3 balance = vec3(
                1.0 + u_temperature * 0.10 - u_tint * 0.05,
                1.0 + u_tint * 0.03,
                1.0 - u_temperature * 0.10 - u_tint * 0.05
        );
        color *= max(balance, 0.0);

        vec3 hsv = rgb_to_hsv(max(color, vec3(0.0)));
        hsv.x = fract(hsv.x + u_hue_shift / 360.0);
        color = hsv_to_rgb(hsv);

        float luma = luminance(color);
        color = mix(vec3(luma), color, max(u_saturation, 0.0));

        float contrast = 1.0 + u_contrast;
        color = (color - vec3(0.18)) * contrast + vec3(0.18);
        return max(color, vec3(0.0));
}

float grain(vec2 uv, float time) {
        return fract(sin(dot(uv + time, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
        vec2 centered_uv = v_uv - 0.5;
        float radial = length(centered_uv);

        vec3 color;
        if (u_chromatic_enabled && u_post_enabled) {
                vec2 offset = centered_uv * u_chromatic_intensity * (1.0 + radial * 2.0);
                color = vec3(
                        sample_scene(v_uv + offset).r,
                        sample_scene(v_uv).g,
                        sample_scene(v_uv - offset).b
                );
        } else {
                color = sample_scene(v_uv);
        }

        if (!u_post_enabled) {
                frag_color = vec4(color, 1.0);
                return;
        }

        color *= exp2(u_exposure);

        if (u_has_bloom)
                color += texture(u_bloom_texture, v_uv).rgb * u_bloom_intensity;

        if (u_color_adjustments_enabled)
                color = apply_color_adjustments(color);

        if (u_tonemap_enabled) {
                if (u_tonemap_operator == 1)
                        color = tonemap_aces(color);
                else if (u_tonemap_operator == 2)
                        color = tonemap_neutral(color);
                else
                        color = color / (color + vec3(1.0));
        }

        if (u_film_grain_enabled) {
                float noise = grain(v_uv * (1.0 / max(u_inv_resolution.x, 0.00001)), u_time) - 0.5;
                float response = mix(1.0, 1.0 - luminance(color), clamp(u_film_grain_response, 0.0, 1.0));
                color += noise * u_film_grain_intensity * response;
        }

        if (u_vignette_enabled) {
                vec2 vignette_uv = abs(centered_uv) * 2.0;
                vignette_uv.x *= mix(1.0, 1.0 + u_vignette_roundness, 0.5);
                float vignette = pow(clamp(1.0 - dot(vignette_uv, vignette_uv), 0.0, 1.0), max(u_vignette_smoothness * 4.0, 0.001));
                color = mix(u_vignette_color, color, mix(1.0, vignette, clamp(u_vignette_intensity, 0.0, 1.0)));
        }

        color = pow(max(color, vec3(0.0)), vec3(1.0 / max(u_gamma, 0.001)));
        frag_color = vec4(color, 1.0);
}
