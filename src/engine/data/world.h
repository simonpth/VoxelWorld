#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include <glm/glm.hpp>

struct ChunkReadHandle {
  Chunk *chunk = nullptr;

  ChunkReadHandle() = default;
  ChunkReadHandle(Chunk *chunk, std::shared_mutex &mtx)
      : chunk(chunk), m_lock(mtx) {}

  explicit operator bool() const { return chunk != nullptr; }

private:
  std::shared_lock<std::shared_mutex> m_lock;
};

struct ChunkWriteHandle {
  Chunk *chunk = nullptr;

  ChunkWriteHandle() = default;
  ChunkWriteHandle(Chunk *chunk, std::shared_mutex &mtx)
      : chunk(chunk), m_lock(mtx) {}

  explicit operator bool() const { return chunk != nullptr; }

private:
  std::unique_lock<std::shared_mutex> m_lock;
};

class World {
public:
  static constexpr int CHUNKHEIGHT = 8;

  World() = default;

  void loadOrGenerateChunk(const ChunkPosition &pos);

  // Sets the block if the chunk exists, otherwise does nothing
  void setBlock(const glm::ivec3 &worldPos, const Block &block);
  Block getBlock(const glm::ivec3 &worldPos);

  ChunkReadHandle getChunkRead(const ChunkPosition &pos);
  ChunkWriteHandle getChunkWrite(const ChunkPosition &pos);

  // Utility functions for converting between world coordinates and chunk/block coordinates
  static glm::ivec3 chunkAndBlockPosToWorldPos(const ChunkPosition &chunkPos, const glm::ivec3 &blockPos) {
    return glm::ivec3(chunkPos.x() * Chunk::SIZE + blockPos.x,
                      chunkPos.y() * Chunk::SIZE + blockPos.y,
                      chunkPos.z() * Chunk::SIZE + blockPos.z);
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
  std::shared_mutex m_globalMutex;
  std::unordered_map<
      ChunkPosition, std::pair<std::unique_ptr<Chunk>, std::unique_ptr<std::shared_mutex>>>
      m_chunks;
};

#endif // WORLD_H