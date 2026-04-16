#version 410 core

layout(location = 0) in uvec2 data;

uniform float planetRadius;

uniform vec3 relativeChunkPos;

uniform float playerWorldY;
uniform vec3 playerPos; // relative to current chunk origin
uniform int warpMode; // 0 = flat, 1 = plane to sphere mapping, 2 = only move y down based on curvature
uniform mat4 vp;

flat out uint blockIdShared;
flat out uint rotationShared;
out vec2 uvShared;
out float horizontalDistance;
out vec3 relPosShared;

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
  6, 2, 5, 1, // 0: +x
  4, 7, 5, 6, // 1: +y
  7, 3, 6, 2, // 2: +z
  4, 0, 7, 3, // 3: -x
  3, 0, 2, 1, // 4: -y
  5, 1, 4, 0  // 5: -z
);

const vec3 debugVertices[4] = vec3[](
  vec3(-0.5f, -0.5f, 0.0f),
  vec3(0.5f, -0.5f, 0.0f),
  vec3(0.0f,  0.5f, 0.0f),
  vec3(-0.5f,  -0.5f, 0.0f)
);

const vec2 UVs[4] = vec2[](
  vec2(0.0f, 0.0f),
  vec2(0.0f, 1.0f),
  vec2(1.0f, 0.0f),
  vec2(1.0f, 1.0f)
);

void main() {
  uint blockID = data.y >> 16;
  uint x = (data.y >> 8) & 0xFFu;
  uint y = data.y & 0xFFu;
  uint z = data.x >> 24;
  uint width = (data.x >> 16) & 0xFFu;
  uint height = (data.x >> 8) & 0xFFu;
  uint rotation = (data.x >> 3) & 0x1Fu;

  vec3 facePos = cornors[indices[rotation * 4u + uint(gl_VertexID)]];
  vec3 relCurrentPlayerChunkPos = facePos + vec3(x, y, z) / 8 + relativeChunkPos; // position relative to the origin of the currentPlayerChunk
  vec3 relPos = relCurrentPlayerChunkPos - playerPos; // position relative to the player

  vec3 finalPos;

  horizontalDistance = length(relPos.xz);

  if(warpMode == 1) {
    // Warp the vertex position to create a spherical world effect
    float theta = horizontalDistance / planetRadius;
    float r = planetRadius + playerWorldY + relPos.y;
    vec2 xzDir = horizontalDistance > 0.0 ? normalize(relPos.xz) : vec2(0.0);

    vec3 sphereCenter = vec3(0, -(planetRadius + playerWorldY), 0);
    vec3 warped = sphereCenter + r * normalize(vec3(sin(theta) * xzDir.x, cos(theta), sin(theta) * xzDir.y));

    finalPos = warped + playerPos; // convert back to relCurrentPlayerChunkPos coordinates
  } else if (warpMode == 2) {
    // Alternative warping method that keeps the vertex at the same horizontal distance but adjusts the height based on the curvature
    finalPos = relCurrentPlayerChunkPos;
    finalPos.y -= (horizontalDistance * horizontalDistance) / (2.0 * planetRadius);
  } else {
    // No warping, flat world
    finalPos = relCurrentPlayerChunkPos;
  }

  gl_Position = vp * vec4(finalPos, 1.0);
  relPosShared = finalPos;

  uvShared = UVs[gl_VertexID];

  blockIdShared = blockID;
  rotationShared = rotation;
}