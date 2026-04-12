#include "chunkgeneration.h"

ChunkGeneration &ChunkGeneration::instance() {
  static ChunkGeneration instance;
  return instance;
}

ChunkGeneration::ChunkGeneration() {
  perlin = FastNoise::New<FastNoise::Perlin>();
  perlin->SetScale(400);

  fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(perlin);
  fractal->SetGain(0.5f);
  fractal->SetWeightedStrength(0.3f);
  fractal->SetLacunarity(2.25f);
  fractal->SetOctaveCount(6);
}

std::unique_ptr<Chunk> ChunkGeneration::generateChunk(const ChunkPosition &pos) {
  auto chunk = std::make_unique<Chunk>();

  std::vector<float> noiseValues(Chunk::AREA);

  if(!fractal) {
    return chunk; // Return empty chunk if generator failed to initialize
  }

  auto minMax = fractal->GenUniformGrid2D(
      noiseValues.data(),
      pos.x * Chunk::SIZE, pos.z * Chunk::SIZE,
      Chunk::SIZE, Chunk::SIZE,
      1.0f, 1.0f,
      seed);

  // Simple terrain generation: create a flat terrain at y=128
  if (pos.y < 0 || pos.y >= World::CHUNKHEIGHT) {
    return chunk; // empty chunk
  }

  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      int worldX = pos.x * Chunk::SIZE + x;
      int worldZ = pos.z * Chunk::SIZE + z;
      int height = noiseValues[x + z * Chunk::SIZE] * 64 + 128; // Scale and shift noise to get height

      for (int y = 0; y < height - pos.y * Chunk::SIZE && y < Chunk::SIZE; ++y) {
        if (y + pos.y * Chunk::SIZE < 135) {
          chunk->setBlock(x, y, z, Block(2)); // Set block type 1
        } else {
          chunk->setBlock(x, y, z, Block(3)); // Set block type 2
        }
      }
    }
  }

  return std::move(chunk);
}