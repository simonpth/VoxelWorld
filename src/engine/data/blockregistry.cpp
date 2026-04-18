#include "blockregistry.h"

BlockRegistry::BlockRegistry() {
  m_blockTextures.emplace_back(glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec4(0.0f, 0.58f, 0.031f, 1.0f)); // Grass block
  m_blockTextureMap.emplace(1, 0);                                                                                       // Block ID 1 maps to texture atlas coordinates (0, 0)

  m_blockTextures.emplace_back(glm::vec2(1, 0), glm::vec4(0.588f, 0.294f, 0.0f, 1.0f)); // Dirt block
  m_blockTextureMap.emplace(2, 1);                                                      // Block ID 2 maps to texture atlas coordinates (1, 0)

  m_blockTextures.emplace_back(glm::vec2(2, 0), glm::vec4(0.467f, 0.467f, 0.467f, 1.0f)); // Stone block
  m_blockTextureMap.emplace(3, 2);                                                        // Block ID 3 maps to texture atlas coordinates (2, 0)

  m_blockTextures.emplace_back(glm::vec2(3, 0), glm::vec4(0.95f, 0.95f, 0.95f, 1.0f)); // Snow block
  m_blockTextureMap.emplace(4, 3);                                                     // Block ID 4 maps to texture atlas coordinates (3, 0)
}