#VERTEX
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform float u_Time;

out float vHeight;

void main() {
    vec3 worldPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));

    // Large-scale field parameters
    float amplitude = 10.0;     // BIG vertical movement
    float frequency = 0.08;     // Large features
    float speed     = 2.2;     // Breathing speed

    // Coherent traveling field
    float field =
          sin(worldPos.x * frequency + u_Time * speed)
        + sin(worldPos.z * frequency - u_Time * speed);

    field *= 0.5;

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
    float t = vHeight * 0.5 + 0.5;

    // Clean energy gradient (smooth, non-abrupt)
    vec3 low  = vec3(0.1, 0.15, 0.55);
    vec3 mid  = vec3(0.8, 0.2, 0.0);
    vec3 high = vec3(0.0, 0.95, 1.0);

    vec3 color = mix(low, mid, t);
    color = mix(color, high, t * t);

    FragColor = vec4(color, 1.0);
}
