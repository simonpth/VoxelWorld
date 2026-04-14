#ifndef BLOCKREGISTRY_H
#define BLOCKREGISTRY_H

#include <glm/glm.hpp>

struct BlockTexture {
  glm::vec2 atlasCoords[6]; // Texture atlas coordinates for each face of the block
  glm::vec4 color;           // Color tint for the block

  BlockTexture(glm::vec2 tex, glm::vec4 col) {
    atlasCoords[0] = tex; // right
    atlasCoords[1] = tex; // top
    atlasCoords[2] = tex; // front
    atlasCoords[3] = tex; // left
    atlasCoords[4] = tex; // bottom
    atlasCoords[5] = tex; // back
    color = col;
  }

  BlockTexture(glm::vec2 top, glm::vec2 side, glm::vec2 bottom, glm::vec4 col) {
    atlasCoords[0] = side; // right
    atlasCoords[1] = top;  // top
    atlasCoords[2] = side; // front
    atlasCoords[3] = side; // left
    atlasCoords[4] = bottom; // bottom
    atlasCoords[5] = side; // back
    color = col;
  }
};

class BlockRegistry {
public:
  BlockRegistry();

  uint32_t getAtlasIndex(uint32_t blockID) const {
    auto it = m_blockTextureMap.find(blockID);
    if (it != m_blockTextureMap.end()) {
      return it->second;
    }
    assert(false && "Block ID not found in registry");
    return 0; // Default to first texture if block ID not found
  }

  const std::vector<BlockTexture>& getBlockTextures() const {
    return m_blockTextures;
  }

private:
  std::vector<BlockTexture> m_blockTextures; // only write to it in the constructor, read-only afterwards
  std::unordered_map<uint32_t, uint32_t> m_blockTextureMap; // Map block ID to texture atlas coordinates
};

#endif // BLOCKREGISTRY_H