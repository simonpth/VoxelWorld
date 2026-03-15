#include "chunkgeneration.h"
#include "chunk.h"
#include "world.h"

std::unique_ptr<Chunk>
ChunkGeneration::generateChunk(const ChunkPosition &pos) {
  auto chunk = std::make_unique<Chunk>();

  // Simple terrain generation: create a flat terrain at y=128
  if (pos.y < 0 || pos.y >= World::CHUNKHEIGHT) {
    return chunk; // empty chunk
  }

  int blockId = (pos.y % 3) + 1; // Just for variety, use different block types based on height

  if (pos.y < 4 && pos.y > 0) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int x = 0; x < Chunk::SIZE; ++x) {
          chunk->setBlock(x, y, z, Block(blockId));
        }
      }
    }
  } else {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int x = 0; x < Chunk::SIZE; ++x) {
          if ((x - 16) * (x - 16) + (y - 16) * (y - 16) + (z - 16) * (z - 16) <
              10 * 10)
            chunk->setBlock(x, y, z, Block(blockId));
        }
      }
    }
  }

  return std::move(chunk);
}