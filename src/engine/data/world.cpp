#include "world.h"

#include "engine/algorithm/chunkgeneration.h"

void World::loadOrGenerateChunk(const ChunkPosition &pos) {
  if (pos.y < 0 || pos.y >= CHUNKHEIGHT)
    return;

  {
    std::shared_lock lock(m_globalMutex);
    if (m_chunks.contains(pos))
      return;
  }

  // Generate outside the lock — this is expensive
  auto chunk = ChunkGeneration::instance().generateChunk(pos);

  std::unique_lock lock(m_globalMutex);
  // Check again — another thread may have inserted while generating
  m_chunks.try_emplace(pos,
                       std::move(chunk),
                       std::make_unique<std::shared_mutex>());
}

void World::setBlock(const glm::ivec3 &worldPos, const Block &block) {
  auto [chunkPos, pos] = worldPosToChunkAndBlockPos(worldPos);

  if (auto chunkWriteHandle = getChunkWrite(chunkPos)) {
    chunkWriteHandle.chunk->setBlock(pos.x, pos.y, pos.z, block);
  }
}

Block World::getBlock(const glm::ivec3 &worldPos) {
  // Similar to setBlock but returns the block at the given world position
  auto [chunkPos, pos] = worldPosToChunkAndBlockPos(worldPos);

  if (auto chunkReadHandle = getChunkRead(chunkPos)) {
    Block block = chunkReadHandle.chunk->block(pos.x, pos.y, pos.z);
    return block;
  }
  return Block();
}

ChunkReadHandle World::getChunkRead(const ChunkPosition &pos) {
  std::shared_lock lock(m_globalMutex);
  if (auto it = m_chunks.find(pos); it != m_chunks.end()) {
    return ChunkReadHandle(it->second.first.get(), *it->second.second);
  }
  return ChunkReadHandle();
}

ChunkWriteHandle World::getChunkWrite(const ChunkPosition &pos) {
  std::shared_lock lock(m_globalMutex);
  if (auto it = m_chunks.find(pos); it != m_chunks.end()) {
    return ChunkWriteHandle(it->second.first.get(), *it->second.second);
  }
  return ChunkWriteHandle();
}