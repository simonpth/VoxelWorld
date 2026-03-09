#version 330 core
layout(location = 0) in uvec2 data;

uniform vec3 relativeChunkPos;
uniform mat4 mvp_matrix;

flat out uint blockIDOut;

// array of 6 * 4 vertices (x, y, z) for each block face instance
const vec3 vertices[24] = vec3[](
    // 0: +x
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0),

    // 1: +y
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 0.0),

    // 2: +z
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 0.0, 1.0),

    // 3: -x
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 0.0),

    // 4: -y
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 1.0),

    // 5: -z
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 0.0)
);

void main() {
    uint blockID = data.y >> 16;
    uint x = (data.y >> 8) & 0xFFu;
    uint y = data.y & 0xFFu;
    uint z = data.x >> 24;
    uint width = (data.x >> 16) & 0xFFu;
    uint height = (data.x >> 8) & 0xFFu;
    uint rotation = (data.x >> 3) & 0x1Fu;

    //gl_Position = mvp_matrix * vec4(vertices[rotation * 4u + uint(gl_InstanceID)] + vec3(x, y, z) + relativeChunkPos, 1.0);
    gl_Position = mvp_matrix * vec4(vertices[gl_InstanceID], 1.0);
    blockIDOut = blockID;
}