#include "world.h"
#include "chunk.h"
#include "chunkgeneration.h"
#include <QtCore/qreadwritelock.h>

std::unique_ptr<ChunkMeshingData>
World::requestChunkMeshingData(const ChunkPosition &pos) {
  // generate needed chunks if they don't exist
  loadOrGenerateChunk(pos);
  loadOrGenerateChunk(pos + ChunkPosition(1, 0, 0));
  loadOrGenerateChunk(pos + ChunkPosition(0, 1, 0));
  loadOrGenerateChunk(pos + ChunkPosition(0, 0, 1));
  loadOrGenerateChunk(pos + ChunkPosition(-1, 0, 0));
  loadOrGenerateChunk(pos + ChunkPosition(0, -1, 0));
  loadOrGenerateChunk(pos + ChunkPosition(0, 0, -1));

  // Now we can safely read the chunk data
  QReadLocker locker(&m_chunksLock);
  auto meshingData = std::make_unique<ChunkMeshingData>();

  meshingData->chunk = *m_chunks[pos];

  // Fill in the solid masks based on the chunk's blocks
  for (int x = 0; x < Chunk::SIZE; ++x) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        if (meshingData->chunk.block(x, y, z).isSolid()) {
          meshingData->setSolidMask(x, y, z);
        }
      }
    }
  }

  // Masks are 34x34x34 to account for neighboring blocks, so we need to check
  // adjacent chunks
  // Check +X neighbor
  Chunk *neighborChunk = m_chunks[pos + ChunkPosition(1, 0, 0)].get();
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      if (neighborChunk->block(0, y, z).isSolid()) {
        meshingData->setSolidMask(Chunk::SIZE, y, z); // +X face
      }
    }
  }
  // Check +Y neighbor
  Chunk *neighborChunkY = m_chunks[pos + ChunkPosition(0, 1, 0)].get();
  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkY->block(x, 0, z).isSolid()) {
        meshingData->setSolidMask(x, Chunk::SIZE, z); // +Y face
      }
    }
  }
  // Check +Z neighbor
  Chunk *neighborChunkZ = m_chunks[pos + ChunkPosition(0, 0, 1)].get();
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkZ->block(x, y, 0).isSolid()) {
        meshingData->setSolidMask(x, y, Chunk::SIZE); // +Z face
      }
    }
  }
  // Check -X neighbor
  const Chunk *neighborChunkNegX =
      m_chunks[pos + ChunkPosition(-1, 0, 0)].get();
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      if (neighborChunkNegX->block(Chunk::SIZE - 1, y, z).isSolid()) {
        meshingData->setSolidMask(-1, y, z); // -X face
      }
    }
  }

  // Check -Y neighbor
  const Chunk *neighborChunkNegY =
      m_chunks[pos + ChunkPosition(0, -1, 0)].get();
  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkNegY->block(x, Chunk::SIZE - 1, z).isSolid()) {
        meshingData->setSolidMask(x, -1, z); // -Y face
      }
    }
  }
  // Check -Z neighbor
  const Chunk *neighborChunkNegZ =
      m_chunks[pos + ChunkPosition(0, 0, -1)].get();
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      if (neighborChunkNegZ->block(x, y, Chunk::SIZE - 1).isSolid()) {
        meshingData->setSolidMask(x, y, -1); // -Z face
      }
    }
  }

  return std::move(meshingData);
}

void World::loadOrGenerateChunk(const ChunkPosition &pos) {
  QReadLocker readLocker(&m_chunksLock);
  if (m_chunks.find(pos) != m_chunks.end())
    return; // Chunk already exists, no need to generate
  readLocker.unlock();

  QWriteLocker locker(&m_chunksLock);
  if (m_chunks.find(pos) == m_chunks.end()) {
    m_chunks[pos] = ChunkGeneration::generateChunk(pos);
  }
}