#include "world.h"

#include "engine/algorithm/chunkgeneration.h"

void World::loadOrGenerateChunk(const ChunkPosition &pos) {
  std::shared_lock lock(m_chunksMutex);
  if (m_chunks.find(pos) != m_chunks.end())
    return; // Chunk already exists, no need to generate
  lock.unlock();

  std::unique_lock uniqueLock(m_chunksMutex);
  if (m_chunks.find(pos) == m_chunks.end()) {
    m_chunks[pos] = ChunkGeneration::generateChunk(pos);
  }
}