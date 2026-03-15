#include "world.h"
#include "chunkgeneration.h"

// This function should only be called while holding the m_chunksMutex
Chunk *World::chunkAtWithLoadOrGenerate(const ChunkPosition &pos) {
  auto it = m_chunks.find(pos);
  if (it == m_chunks.end()) {
    return loadOrGenerateChunk(pos);
  } else {
    return it->second.get();
  }
}

std::unique_ptr<ChunkRenderData>
World::requestChunkRenderData(const ChunkPosition &pos) {
  QMutexLocker locker(&m_chunksMutex);
  auto renderData = std::make_unique<ChunkRenderData>();
  renderData->chunk = *(chunkAtWithLoadOrGenerate(pos));

  // Fill in the solid masks based on the chunk's blocks
  for (int x = 0; x < Chunk::SIZE; ++x) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        if (renderData->chunk.block(x, y, z).isSolid()) {
          renderData->setSolidMask(x, y, z);
        }
      }
    }
  }

  // Masks are 34x34x34 to account for neighboring blocks, so we need to check
  // adjacent chunks
  // Check +X neighbor
  Chunk *neighborChunk =
      chunkAtWithLoadOrGenerate(pos + ChunkPosition(1, 0, 0));
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      if (neighborChunk->block(0, y, z).isSolid()) {
        renderData->setSolidMask(Chunk::SIZE, y, z); // +X face
      }
    }
  }
  // Check +Y neighbor
  Chunk *neighborChunkY =
      chunkAtWithLoadOrGenerate(pos + ChunkPosition(0, 1, 0));
  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkY->block(x, 0, z).isSolid()) {
        renderData->setSolidMask(x, Chunk::SIZE, z); // +Y face
      }
    }
  }
  // Check +Z neighbor
  Chunk *neighborChunkZ =
      chunkAtWithLoadOrGenerate(pos + ChunkPosition(0, 0, 1));
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkZ->block(x, y, 0).isSolid()) {
        renderData->setSolidMask(x, y, Chunk::SIZE); // +Z face
      }
    }
  }
  // Check -X neighbor
  const Chunk *neighborChunkNegX =
      chunkAtWithLoadOrGenerate(pos + ChunkPosition(-1, 0, 0));
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      if (neighborChunkNegX->block(Chunk::SIZE - 1, y, z).isSolid()) {
        renderData->setSolidMask(-1, y, z); // -X face
      }
    }
  }

  // Check -Y neighbor
  const Chunk *neighborChunkNegY =
      chunkAtWithLoadOrGenerate(pos + ChunkPosition(0, -1, 0));
  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkNegY->block(x, Chunk::SIZE - 1, z).isSolid()) {
        renderData->setSolidMask(x, -1, z); // -Y face
      }
    }
  }
  // Check -Z neighbor
  const Chunk *neighborChunkNegZ =
      chunkAtWithLoadOrGenerate(pos + ChunkPosition(0, 0, -1));
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkNegZ->block(x, y, Chunk::SIZE - 1).isSolid()) {
        renderData->setSolidMask(x, y, -1); // -Z face
      }
    }
  }

  return std::move(renderData);
}

// This function should only be called while holding the m_chunksMutex
Chunk *World::loadOrGenerateChunk(const ChunkPosition &pos) {
  if (m_chunks.find(pos) == m_chunks.end()) {
    m_chunks[pos] = ChunkGeneration::generateChunk(pos);
  }
  return m_chunks[pos].get();
}