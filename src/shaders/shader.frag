#version 410 core

flat in uint blockIdShared;
flat in uint rotationShared;

in vec3 relFragPos;

uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;

uniform samplerBuffer blockTextureTBO;

out vec4 FragColor;

// calculate lighting based on face rotation (simplified)

vec3 lightDir = vec3(0.3, 1.0, 0.2); // light coming from above and to the side

vec3 normals[6] = vec3[](
  vec3(1.0, 0.0, 0.0),  // x+
  vec3(0.0, 1.0, 0.0),  // y+
  vec3(0.0, 0.0, 1.0),  // z+
  vec3(-1.0, 0.0, 0.0), // x-
  vec3(0.0, -1.0, 0.0), // y-
  vec3(0.0, 0.0, -1.0)  // z-
);

float getLighting(uint rotation) {
  vec3 normal = normals[rotation];
  float diff = max(dot(normal, lightDir), 0.0);
  return diff * 0.4 + 0.7; // simple diffuse + ambient
}

void main() {
  uint baseIndex = blockIdShared * 4; // 6 faces per block

  vec4 baseColor = texelFetch(blockTextureTBO, int(baseIndex + 3)).rgba; // base color

  vec3 color = baseColor.rgb * getLighting(rotationShared);

  vec3 vertPos = relFragPos; // position of the fragment in world space
  vertPos.y = 0.0;
  float distance = length(vertPos); // distance from camera in xz-plane
  float fogFactor = clamp((distance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);

  color = mix(color, fogColor, fogFactor);

  FragColor = vec4(color, baseColor.a);
}
