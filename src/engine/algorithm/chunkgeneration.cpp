#include "chunkgeneration.h"
#include "engine/settings.h"

ChunkGeneration &ChunkGeneration::instance() {
  static ChunkGeneration instance;
  return instance;
}

ChunkGeneration::ChunkGeneration() {
  generator = FastNoise::NewFromEncodedNodeTree("FQkXCSAJBg@AHpEE@DYmpnZPwwCKQkNAAc@BJC@BEhEHAL/AwAE");

  snow = FastNoise::New<FastNoise::Simplex>();
  snow->SetScale(400.0f);

  m_seed = Settings::instance().worldSeed();
  m_planetSizeInChunks = Settings::instance().planetSizeInChunks();
}

std::unique_ptr<Chunk> ChunkGeneration::generateChunk(const ChunkPosition &pos) {
  auto chunk = std::make_unique<Chunk>();

  std::vector<float> heightValues(Chunk::AREA);
  std::vector<float> snowValues(Chunk::AREA);

  generator->GenUniformGrid2D(
      heightValues.data(),
      pos.x() * Chunk::SIZE, pos.z() * Chunk::SIZE,
      Chunk::SIZE, Chunk::SIZE,
      1.0f, 1.0f,
      m_seed);

  snow->GenUniformGrid2D(
      snowValues.data(),
      pos.x() * Chunk::SIZE, pos.z() * Chunk::SIZE,
      Chunk::SIZE, Chunk::SIZE,
      1.0f, 1.0f,
      m_seed + 1);

  // Simple terrain generation: create a flat terrain at y=128
  if (pos.y() < 0 || pos.y() >= World::CHUNKHEIGHT) {
    return chunk; // empty chunk
  }

  float edgeDistance = 64.0f; // Distance from the edge over which to apply the falloff

  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      int worldX = pos.x() * Chunk::SIZE + x;
      int worldZ = pos.z() * Chunk::SIZE + z;

      // Apply a falloff to the height values near the edges of the world
      if (worldX < edgeDistance) {
        heightValues[x + z * Chunk::SIZE] *= std::pow(worldX/edgeDistance, 2.0f);
      } else if (worldX > m_planetSizeInChunks * Chunk::SIZE - edgeDistance) {
        heightValues[x + z * Chunk::SIZE] *= std::pow((m_planetSizeInChunks * Chunk::SIZE - worldX)/edgeDistance, 2.0f);
      }
      if (worldZ < edgeDistance) {
        heightValues[x + z * Chunk::SIZE] *= std::pow(worldZ/edgeDistance, 2.0f);
      } else if (worldZ > m_planetSizeInChunks * Chunk::SIZE - edgeDistance) {
        heightValues[x + z * Chunk::SIZE] *= std::pow((m_planetSizeInChunks * Chunk::SIZE - worldZ)/edgeDistance, 2.0f);
      }

      int height = heightValues[x + z * Chunk::SIZE] * 48 + 96; // Scale and shift noise to get height

      int snowHeight = snowValues[x + z * Chunk::SIZE] * 4;

      if (height < 5)
        height = 5; // Ensure there's at least some ground

      for (int y = 0; y <= height - pos.y() * Chunk::SIZE && y < Chunk::SIZE; ++y) {
        if (y + pos.y() * Chunk::SIZE > 137 + snowHeight) {
          chunk->setBlock(x, y, z, Block(4)); // Snow
        } else if (y + pos.y() * Chunk::SIZE > 130 + snowHeight) {
          chunk->setBlock(x, y, z, Block(3)); // Stone
        } else if (y + pos.y() * Chunk::SIZE == height) {
          chunk->setBlock(x, y, z, Block(1)); // Grass
        } else if (y + pos.y() * Chunk::SIZE >= height - 5) {
          chunk->setBlock(x, y, z, Block(2)); // Dirt
        } else {
          chunk->setBlock(x, y, z, Block(3)); // Stone
        }
      }
    }
  }

  return chunk;
}