#ifndef CHUNKGENERATION_H
#define CHUNKGENERATION_H

#include "engine/data/world.h"
#include <FastNoise/FastNoise.h>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

struct HeightMap {
  std::vector<float> heightValues;
  std::vector<float> snowValues;

  HeightMap() {
    heightValues.resize(Chunk::AREA);
    snowValues.resize(Chunk::AREA);
  }
};

class ChunkGeneration {
public:
  static ChunkGeneration &instance();

  std::unique_ptr<Chunk> generateChunk(const ChunkPosition &pos);

private:
  ChunkGeneration();

  void generateHeightmap(const ChunkPosition &pos);

  FastNoise::SmartNode<> m_generator;
  FastNoise::SmartNode<FastNoise::Simplex> m_snow;
  int m_seed;               // Store the seed used for generation
  int m_planetSizeInChunks; // Store the planet size in chunks for calculating world wrapping

  // Guards access to both maps below
  std::mutex m_mapMutex;
  std::unordered_map<ChunkPosition, std::shared_ptr<std::once_flag>> m_heightmapGenerationFlags;
  std::unordered_map<ChunkPosition, std::shared_ptr<HeightMap>> m_heightmapCache;
  
  // Guards reads/writes to the cache itself
  std::shared_mutex m_cacheMutex;
};

#endif // CHUNKGENERATION_H