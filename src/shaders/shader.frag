#version 410 core

flat in uint blockIdShared;
flat in uint rotationShared;
in vec2 uvShared;

flat in uint highlightBlockShared;

in float horizontalDistance;
in vec3 relPosShared;

uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;

uniform bool useTextures;

uniform samplerBuffer blockTextureTBO;
uniform sampler2D blockTextureAtlas;

// Texture fade-out parameters
uniform float textureFadeDistance = 256; // Distance at which textures start fading
uniform float textureFadeStrength = 1.5;   // How quickly textures fade out (higher = faster fade)

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

  vec2 baseUV;
  if (rotationShared == 0u) {
    baseUV = texelFetch(blockTextureTBO, int(baseIndex)).rg; // x+ face UV
  } else if (rotationShared == 1u) {
    baseUV = texelFetch(blockTextureTBO, int(baseIndex)).ba; // y+ face UV
  } else if (rotationShared == 2u) {
    baseUV = texelFetch(blockTextureTBO, int(baseIndex + 1)).rg; // z+ face UV
  } else if (rotationShared == 3u) {
    baseUV = texelFetch(blockTextureTBO, int(baseIndex + 1)).ba; // x- face UV
  } else if (rotationShared == 4u) {
    baseUV = texelFetch(blockTextureTBO, int(baseIndex + 2)).rg; // y- face UV
  } else if (rotationShared == 5u) {
    baseUV = texelFetch(blockTextureTBO, int(baseIndex + 2)).ba; // z- face UV
  }

  vec2 atlasUV = (baseUV + clamp(uvShared, 0.05, 0.95)) / 64.0; // 64x64 blocks in atlas, clamp to avoid bleeding

  vec4 baseColor;
  vec4 textureColor = texelFetch(blockTextureTBO, int(baseIndex + 3)).rgba; // base color

  if(useTextures) {
    // Calculate texture fade factor based on distance
    float distance = length(relPosShared);
    float textureFadeFactor = 1.0;

    if(distance > textureFadeDistance) {
      float fadeRange = distance - textureFadeDistance;
      textureFadeFactor = exp(-textureFadeStrength * fadeRange / textureFadeDistance);
      textureFadeFactor = clamp(textureFadeFactor, 0.0, 1.0);
    }

    // Only fetch texture if we actually need it (fade factor > 0)
    if(textureFadeFactor > 0.0) {
      vec4 atlasColor = texture(blockTextureAtlas, atlasUV);
      // Blend between solid color and atlas texture based on fade factor
      baseColor = mix(textureColor, atlasColor, textureFadeFactor);
    } else {
      baseColor = textureColor;
    }
  } else {
    baseColor = textureColor;
  }

  vec3 color = baseColor.rgb * getLighting(rotationShared);

  float fogFactor = clamp((horizontalDistance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
  color = mix(color, fogColor, fogFactor);

  if (highlightBlockShared == 1u) {
    if (uvShared.x < 0.05 || uvShared.x > 0.95 || uvShared.y < 0.05 || uvShared.y > 0.95) {
      color = mix(color, vec3(0.1, 0.1, 0.1), 0.8); // black for highlight outline
    }
  }

  FragColor = vec4(color, baseColor.a);
}
