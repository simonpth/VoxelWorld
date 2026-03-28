#include "world.h"

#include "engine/algorithm/chunkgeneration.h"

void World::loadOrGenerateChunk(const ChunkPosition &pos) {
  if(pos.y < 0 || pos.y >= CHUNKHEIGHT)
    return; // Out of world bounds

  std::shared_lock lock(m_chunksMutex);
  if (m_chunks.contains(pos))
    return; // Chunk already exists, no need to generate
  lock.unlock();

  std::unique_lock uniqueLock(m_chunksMutex);
  if (m_chunks.find(pos) == m_chunks.end()) {
    m_chunks[pos] = ChunkGeneration::generateChunk(pos);
  }
}

void World::setBlock(const glm::ivec3 &worldPos, const Block &block) {
  auto [chunkPos, pos] = worldPosToChunkAndBlockPos(worldPos);

  std::shared_lock lock(m_chunksMutex);
  if (m_chunks.contains(chunkPos)) {
    lock.unlock();
    std::unique_lock uniqueLock(m_chunksMutex);
    m_chunks.at(chunkPos)->setBlock(pos.x, pos.y, pos.z, block);
  }
}