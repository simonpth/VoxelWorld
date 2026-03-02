#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

#include <memory.h>
#include <memory>
#include <unordered_map>

class World {
public:
  World() = default;

  Chunk *chunk(const ChunkPosition &pos) {
    auto it = m_chunks.find(pos);
    if (it != m_chunks.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  void setChunk(const ChunkPosition &pos, std::unique_ptr<Chunk> chunk) {
    m_chunks[pos] = std::move(chunk);
  }

private:
  std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks;
};

#endif // WORLD_H