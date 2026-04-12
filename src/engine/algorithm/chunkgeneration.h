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
  
  FastNoise::SmartNode<FastNoise::Perlin> perlin;
  FastNoise::SmartNode<FastNoise::FractalFBm> fractal;
  int seed = 1337;
};

#endif // CHUNKGENERATION_H