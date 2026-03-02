#ifndef CHUNKGENERATION_H
#define CHUNKGENERATION_H

#include "chunk.h"
#include <memory>

class ChunkGeneration {
  public:
    static std::unique_ptr<Chunk> generateChunk(const ChunkPosition &pos);
};

#endif // CHUNKGENERATION_H