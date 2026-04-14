#include "chunkgeneration.h"

ChunkGeneration &ChunkGeneration::instance() {
  static ChunkGeneration instance;
  return instance;
}

ChunkGeneration::ChunkGeneration() {
  generator = FastNoise::NewFromEncodedNodeTree("FQkXCSAJBg@AHpEE@DYmpnZPwwCKQkNAAc@BJC@BEhEHAL/AwAE");

  snow = FastNoise::New<FastNoise::Simplex>();
  snow->SetScale(400.0f);
}

std::unique_ptr<Chunk> ChunkGeneration::generateChunk(const ChunkPosition &pos) {
  auto chunk = std::make_unique<Chunk>();

  std::vector<float> heightValues(Chunk::AREA);
  std::vector<float> snowValues(Chunk::AREA);

  generator->GenUniformGrid2D(
      heightValues.data(),
      pos.x * Chunk::SIZE, pos.z * Chunk::SIZE,
      Chunk::SIZE, Chunk::SIZE,
      1.0f, 1.0f,
      seed);

  snow->GenUniformGrid2D(
      snowValues.data(),
      pos.x * Chunk::SIZE, pos.z * Chunk::SIZE,
      Chunk::SIZE, Chunk::SIZE,
      1.0f, 1.0f,
      seed + 1);

  // Simple terrain generation: create a flat terrain at y=128
  if (pos.y < 0 || pos.y >= World::CHUNKHEIGHT) {
    return chunk; // empty chunk
  }

  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      int worldX = pos.x * Chunk::SIZE + x;
      int worldZ = pos.z * Chunk::SIZE + z;
      int height = heightValues[x + z * Chunk::SIZE] * 48 + 64; // Scale and shift noise to get height
      int snowHeight = snowValues[x + z * Chunk::SIZE] * 4;

      if(height < 5) height = 5; // Ensure there's at least some ground

      for (int y = 0; y <= height - pos.y * Chunk::SIZE && y < Chunk::SIZE; ++y) {
        if (y + pos.y * Chunk::SIZE > 137 + snowHeight) {
          chunk->setBlock(x, y, z, Block(4)); // Snow
        } else if (y + pos.y * Chunk::SIZE > 130 + snowHeight) {
          chunk->setBlock(x, y, z, Block(3)); // Stone
        } else if (y + pos.y * Chunk::SIZE == height) {
          chunk->setBlock(x, y, z, Block(1)); // Grass
        } else if (y + pos.y * Chunk::SIZE >= height - 5) {
          chunk->setBlock(x, y, z, Block(2)); // Dirt
        } else {
          chunk->setBlock(x, y, z, Block(3)); // Stone
        }
      }
    }
  }

  return std::move(chunk);
}