#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

#include <memory>
#include <unordered_map>
#include <shared_mutex>

class World {
public:
  static constexpr int CHUNKHEIGHT = 8;

  World() = default;

  void loadOrGenerateChunk(const ChunkPosition &pos);

  void lockChunksMutexShared() { m_chunksMutex.lock_shared(); }
  void unlockChunksMutexShared() { m_chunksMutex.unlock_shared(); }

  void lockChunksMutexUnique() { m_chunksMutex.lock(); }
  void unlockChunksMutexUnique() { m_chunksMutex.unlock(); }

  const std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> &chunks() const { return m_chunks; }
  
private:
  std::shared_mutex m_chunksMutex;
  std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks;
};

#endif // WORLD_H