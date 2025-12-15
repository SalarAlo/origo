#VERTEX
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform float u_Time;

out float vHeight;

// 2D rotation
mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

// Cheap hash (stable per cube)
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

void main() {
    vec3 worldPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));

    // Work in XZ plane
    vec2 p = worldPos.xz;

    // Rotate space to kill grid alignment
    p = rot(0.73) * p;

    // --- DOMAIN WARPING ---
    float warp =
          sin(p.x * 0.05 + u_Time * 1.3)
        + sin(p.y * 0.05 - u_Time * 1.1);

    p += vec2(cos(warp), sin(warp)) * 4.0;

    // --- MULTI-OCTAVE COHERENT FIELD ---
    float field = 0.0;

    field += sin(p.x * 0.08 + u_Time * 2.2);
    field += sin(p.y * 0.11 - u_Time * 1.7) * 0.7;
    field += sin((p.x + p.y) * 0.04 + u_Time * 0.9) * 1.3;

    field /= 3.0;

    // --- PER-CELL PHASE DESYNC ---
    // Uses coarse grid so continuity remains
    vec2 cell = floor(worldPos.xz * 0.1);
    float phase = hash(cell) * 6.2831853;

    field += sin(u_Time * 1.8 + phase) * 0.25;

    // Final vertical displacement
    float amplitude = 10.0;
    worldPos.y += field * amplitude;

    vHeight = field;

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(worldPos, 1.0);
}

#FRAGMENT
#version 430 core

in float vHeight;
out vec4 FragColor;

void main() {
    // Normalize to 0..1
    float t = clamp(vHeight * 0.5 + 0.5, 0.0, 1.0);

    // Energetic but smooth gradient
    vec3 low  = vec3(0.45, 0.10, 0.35);
    vec3 mid  = vec3(0.90, 0.25, 0.05);
    vec3 high = vec3(0.2, 0.95, 1.0);

    vec3 color = mix(low, mid, t);
    color = mix(color, high, t * t);

    FragColor = vec4(color, 1.0);
}
