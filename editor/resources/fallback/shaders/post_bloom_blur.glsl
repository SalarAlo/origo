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

uniform sampler2D u_image;
uniform vec2 u_direction;

void main() {
        vec3 color = texture(u_image, v_uv).rgb * 0.227027;
        color += texture(u_image, v_uv + u_direction * 1.384615).rgb * 0.316216;
        color += texture(u_image, v_uv - u_direction * 1.384615).rgb * 0.316216;
        color += texture(u_image, v_uv + u_direction * 3.230769).rgb * 0.070270;
        color += texture(u_image, v_uv - u_direction * 3.230769).rgb * 0.070270;
        frag_color = vec4(color, 1.0);
}
