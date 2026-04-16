#ifndef CHUNKGENERATION_H
#define CHUNKGENERATION_H

#include "engine/data/world.h"
#include <FastNoise/FastNoise.h>
#include <memory>

class ChunkGeneration {
public:
  static ChunkGeneration &instance();
  
  std::unique_ptr<Chunk> generateChunk(const ChunkPosition &pos);

private:
  ChunkGeneration();
  
  FastNoise::SmartNode<> generator;
  FastNoise::SmartNode<FastNoise::Simplex> snow;
  int m_seed; // Store the seed used for generation
  int m_planetSizeInChunks; // Store the planet size in chunks for calculating world wrapping
};

#endif // CHUNKGENERATION_H