#version 330 core

flat in uint blockIdShared;
flat in uint rotationShared;

out vec4 FragColor;

vec3 colors[4] = vec3[](
  vec3(0.5, 0.5, 0.5), // gray for block ID 0
  vec3(1.0, 0.0, 0.0), // red
  vec3(0.0, 1.0, 0.0), // green
  vec3(0.0, 0.0, 1.0)  // blue
);

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
  return diff * 0.4 + 0.6; // simple diffuse + ambient
}

void main() {
  //FragColor = vec4(colors[blockIdShared] * getLighting(rotationShared), 1.0);
  FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
