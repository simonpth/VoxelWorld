#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include <QReadWriteLock>

#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <unordered_map>
#include <utility>

class World {
public:
  static constexpr int CHUNKHEIGHT = 16;

  World() = default;

  Chunk chunk(const ChunkPosition &pos) {
    QReadLocker locker(&m_chunksMutex);
    auto it = m_chunks.find(pos);
    return it != m_chunks.end() ? *(it->second.get()) : Chunk();
  }

  std::pair<Chunk, std::unique_ptr<uint32_t[]>> chunkForMesh(const ChunkPosition &pos);

  void setChunk(const ChunkPosition &pos, std::unique_ptr<Chunk> chunk) {
    QWriteLocker locker(&m_chunksMutex);
    m_chunks[pos] = std::move(chunk);
  }

private:
  QReadWriteLock m_chunksMutex;
  std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks;
};

#endif // WORLD_H