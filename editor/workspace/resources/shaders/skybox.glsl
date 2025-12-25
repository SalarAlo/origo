#VERTEX

#version 330 core
out vec3 v_Dir;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

const vec2 verts[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

void main()
{
    vec2 pos = verts[gl_VertexID];
    gl_Position = vec4(pos, 0.0, 1.0);

    // Ray in View-Space
vec4 clip = vec4(pos, -1.0, 1.0);
vec4 view = inverse(u_ProjectionMatrix) * clip;
view.xyz /= view.w;

    // nur Rotation aus View
    mat3 rot = transpose(mat3(u_ViewMatrix));
    v_Dir = rot * view.xyz;
}

#FRAGMENT

#version 330 core
in vec3 v_Dir;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, normalize(v_Dir));
}
