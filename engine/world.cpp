#include "world.h"
#include "chunk.h"

std::pair<Chunk, std::unique_ptr<uint32_t[]>> World::chunkForMesh(const ChunkPosition &pos) {
  QReadLocker locker(&m_chunksMutex);
  auto it = m_chunks.find(pos);
  if (it == m_chunks.end()) {
    return {Chunk(), nullptr};
  }

  std::unique_ptr<uint32_t[]> nextChunkSolids(new uint32_t[6 * Chunk::SIZE]());

  // Helper lambda to process a face direction
  auto processFace = [&](const ChunkPosition& offset, int baseIndex,
                        auto blockAccess) {
    auto nextPos = pos + offset;
    auto nextIt = m_chunks.find(nextPos);
    if (nextIt != m_chunks.end()) {
      auto* chunk = nextIt->second.get();
      for (int i = 0; i < Chunk::SIZE; ++i) {
        for (int j = 0; j < Chunk::SIZE; ++j) {
          nextChunkSolids[baseIndex + i] |= blockAccess(chunk, i, j) ? (1 << j) : 0;
        }
      }
    }
  };

  // +X face (right) - access blocks at x=0 face
  processFace(ChunkPosition(1, 0, 0), 0,
    [](Chunk* chunk, int y, int z) { return chunk->block(0, y, z).isSolid(); });

  // +Y face (top) - access blocks at y=0 face
  processFace(ChunkPosition(0, 1, 0), Chunk::SIZE,
    [](Chunk* chunk, int z, int x) { return chunk->block(x, 0, z).isSolid(); });

  // +Z face (front) - access blocks at z=0 face
  processFace(ChunkPosition(0, 0, 1), 2 * Chunk::SIZE,
    [](Chunk* chunk, int y, int x) { return chunk->block(x, y, 0).isSolid(); });

  // -X face (left) - access blocks at x=SIZE-1 face
  processFace(ChunkPosition(-1, 0, 0), 3 * Chunk::SIZE,
    [](Chunk* chunk, int y, int z) { return chunk->block(Chunk::SIZE-1, y, z).isSolid(); });

  // -Y face (bottom) - access blocks at y=SIZE-1 face
  processFace(ChunkPosition(0, -1, 0), 4 * Chunk::SIZE,
    [](Chunk* chunk, int z, int x) { return chunk->block(x, Chunk::SIZE-1, z).isSolid(); });

  // -Z face (back) - access blocks at z=SIZE-1 face
  processFace(ChunkPosition(0, 0, -1), 5 * Chunk::SIZE,
    [](Chunk* chunk, int y, int x) { return chunk->block(x, y, Chunk::SIZE-1).isSolid(); });

  return std::make_pair(*(it->second), std::move(nextChunkSolids));
}
