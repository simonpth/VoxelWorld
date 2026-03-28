#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include <glm/glm.hpp>

class World {
public:
  static constexpr int CHUNKHEIGHT = 8;

  World() = default;

  void loadOrGenerateChunk(const ChunkPosition &pos);

  // Sets the block if the chunk exists, otherwise does nothing
  void setBlock(const glm::ivec3 &worldPos, const Block &block);

  void lockChunksMutexShared() { m_chunksMutex.lock_shared(); }
  void unlockChunksMutexShared() { m_chunksMutex.unlock_shared(); }

  void lockChunksMutexUnique() { m_chunksMutex.lock(); }
  void unlockChunksMutexUnique() { m_chunksMutex.unlock(); }

  const std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> &chunks() const { return m_chunks; }

  // Utility functions for converting between world coordinates and chunk/block coordinates
  static glm::ivec3 chunkAndBlockPosToWorldPos(const ChunkPosition &chunkPos, const glm::ivec3 &blockPos) {
    return glm::ivec3(chunkPos.x * Chunk::SIZE + blockPos.x,
                      chunkPos.y * Chunk::SIZE + blockPos.y,
                      chunkPos.z * Chunk::SIZE + blockPos.z);
  }
  static int floorDiv(int a, int b) {
    return a / b - (a % b != 0 && (a ^ b) < 0);
  }
  static int floorMod(int a, int b) {
    int r = a % b;
    return r + (r != 0 && (a ^ b) < 0) * b;
  }
  static std::pair<ChunkPosition, glm::ivec3> worldPosToChunkAndBlockPos(const glm::ivec3 &worldPos) {
    ChunkPosition chunkPos(floorDiv(worldPos.x, Chunk::SIZE), floorDiv(worldPos.y, Chunk::SIZE), floorDiv(worldPos.z, Chunk::SIZE));
    glm::ivec3 blockPos(floorMod(worldPos.x, Chunk::SIZE), floorMod(worldPos.y, Chunk::SIZE), floorMod(worldPos.z, Chunk::SIZE));
    return {chunkPos, blockPos};
  }

private:
  std::shared_mutex m_chunksMutex;
  std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks;
};

#endif // WORLD_H