#version 430 core

in vec2 v_ndc;
out vec4 frag_color;

uniform mat4 u_inv_view_proj;

float grid_strength(float coordinate, float spacing) {
        float scaled = coordinate / spacing;
        float dist = abs(fract(scaled - 0.5) - 0.5);
        float width = max(fwidth(scaled), 0.0001);
        return 1.0 - clamp(dist / width, 0.0, 1.0);
}

float axis_strength(float coordinate) {
        float width = max(fwidth(coordinate) * 2.0, 0.0001);
        return 1.0 - clamp(abs(coordinate) / width, 0.0, 1.0);
}

vec3 world_from_ndc(float z) {
        vec4 world = u_inv_view_proj * vec4(v_ndc, z, 1.0);
        return world.xyz / world.w;
}

void main() {
        vec3 near_point = world_from_ndc(-1.0);
        vec3 far_point = world_from_ndc(1.0);
        vec3 ray = far_point - near_point;

        if (abs(ray.y) < 0.000001) {
                discard;
        }

        float t = -near_point.y / ray.y;
        if (t <= 0.0) {
                discard;
        }

        vec3 world_pos = near_point + t * ray;

        float minor = max(grid_strength(world_pos.x, 1.0), grid_strength(world_pos.z, 1.0));
        float major = max(grid_strength(world_pos.x, 10.0), grid_strength(world_pos.z, 10.0));

        float axis_x = axis_strength(world_pos.z);
        float axis_z = axis_strength(world_pos.x);

        vec3 color = vec3(0.0);
        color += vec3(0.85) * minor;
        color -= vec3(0.1) * major;

        color = mix(color, vec3(0.9, 0.25, 0.25), axis_x);
        color = mix(color, vec3(0.2, 0.55, 0.95), axis_z);

        float alpha = max(max(minor * 0.5, major * 0.95), max(axis_x, axis_z));
        if (alpha <= 0.01) {
                discard;
        }

        gl_FragDepth = 1.0;
        frag_color = vec4(color, clamp(alpha, 0.0, 1.0));
}
