#include "chunkgeneration.h"
#include "chunk.h"

std::unique_ptr<Chunk>
ChunkGeneration::generateChunk(const ChunkPosition &pos) {
  auto chunk = std::make_unique<Chunk>();
  // Simple terrain generation: create a flat terrain at y=256
  if (pos.y < (256 / Chunk::SIZE)) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int x = 0; x < Chunk::SIZE; ++x) {
          chunk->setBlock(x, y, z, 1);
        }
      }
    }
    return chunk;
  } else {
    // Empty chunk
    return chunk;
  }
}