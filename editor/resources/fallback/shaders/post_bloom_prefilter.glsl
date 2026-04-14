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
uniform float u_threshold = 1.0;
uniform float u_knee = 0.5;

float luminance(vec3 color) {
        return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
        vec3 color = texture(u_scene_color, v_uv).rgb;
        float brightness = luminance(color);
        float soft = clamp((brightness - u_threshold + u_knee) / max(2.0 * u_knee, 0.0001), 0.0, 1.0);
        float contribution = max(brightness - u_threshold, 0.0) + soft * soft * u_knee;
        contribution /= max(brightness, 0.0001);
        frag_color = vec4(color * contribution, 1.0);
}
