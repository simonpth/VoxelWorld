#version 410 core

layout(location = 0) in uvec2 data;

uniform vec3 relativeChunkPos;
uniform mat4 vp;

flat out uint blockIdShared;
flat out uint rotationShared;

out vec3 relFragPos;

// array of 6 * 4 vertices (x, y, z) for each block face instance
const vec3 cornors[8] = vec3[](
  vec3(0.0, 0.0, 0.0), // 0
  vec3(1.0, 0.0, 0.0), // 1
  vec3(1.0, 0.0, 1.0), // 2
  vec3(0.0, 0.0, 1.0), // 3
  vec3(0.0, 1.0, 0.0), // 4
  vec3(1.0, 1.0, 0.0), // 5
  vec3(1.0, 1.0, 1.0), // 6
  vec3(0.0, 1.0, 1.0)  // 7
  /*
     4---------5
    /|        /|
   / |       / |
  7--+------6  |
  |  |      |  |
  |  0------|--1
  | /       | /
  |/        |/
  3---------2

      +Y
      |
      |
      |_______ +X
    /
    /
  +Z
  */
);

const int indices[24] = int[](
  2, 1, 6, 5, // 0: +x
  7, 6, 4, 5, // 1: +y
  3, 2, 7, 6, // 2: +z
  3, 0, 7, 4, // 3: -x
  1, 0, 2, 3, // 4: -y
  0, 4, 1, 5  // 5: -z
);

const vec3 debugVertices[4] = vec3[](
  vec3(-0.5f, -0.5f, 0.0f),
  vec3(0.5f, -0.5f, 0.0f),
  vec3(0.0f,  0.5f, 0.0f),
  vec3(-0.5f,  -0.5f, 0.0f)
);

void main() {
  uint blockID = data.y >> 16;
  uint x = (data.y >> 8) & 0xFFu;
  uint y = data.y & 0xFFu;
  uint z = data.x >> 24;
  uint width = (data.x >> 16) & 0xFFu;
  uint height = (data.x >> 8) & 0xFFu;
  uint rotation = (data.x >> 3) & 0x1Fu;

  vec3 facePos = cornors[indices[rotation * 4u + uint(gl_VertexID % 4)]];
  relFragPos = facePos + vec3(x, y, z) / 8 + relativeChunkPos;
  gl_Position = vp * vec4(relFragPos, 1.0);

  blockIdShared = blockID;
  rotationShared = rotation;
}