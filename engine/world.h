#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include <QMutex>
#include <cstdint>
#include <memory>
#include <unordered_map>

struct ChunkRenderData {
  Chunk chunk;
  uint64_t xSolidMasks[Chunk::AREA]; // 32*32 = 1024
  uint64_t ySolidMasks[Chunk::AREA];
  uint64_t zSolidMasks[Chunk::AREA];

  void setSolidMask(int x, int y, int z) {
    if (x >= 0 && x < Chunk::SIZE && y >= 0 && y < Chunk::SIZE && z >= 0 &&
        z < Chunk::SIZE) {
      int index = y * Chunk::SIZE + z; // For x-face
      xSolidMasks[index] |= (1ULL << (x + 1));

      index = z * Chunk::SIZE + x; // For y-face
      ySolidMasks[index] |= (1ULL << (y + 1));

      index = y * Chunk::SIZE + x; // For z-face
      zSolidMasks[index] |= (1ULL << (z + 1));
    } else if (x == -1 || x == Chunk::SIZE) {
      int index = y * Chunk::SIZE + z; // For x-face
      xSolidMasks[index] |= (1ULL << (x + 1));
    } else if (y == -1 || y == Chunk::SIZE) {
      int index = z * Chunk::SIZE + x; // For y-face
      ySolidMasks[index] |= (1ULL << (y + 1));
    } else if (z == -1 || z == Chunk::SIZE) {
      int index = y * Chunk::SIZE + x; // For z-face
      zSolidMasks[index] |= (1ULL << (z + 1));
    }
  }

  ChunkRenderData() {
    for (int i = 0; i < Chunk::AREA; ++i) {
      xSolidMasks[i] = 0;
      ySolidMasks[i] = 0;
      zSolidMasks[i] = 0;
    }
  }
};

class World {
public:
  static constexpr int CHUNKHEIGHT = 8;

  World() = default;

  Chunk *chunkAtWithLoadOrGenerate(const ChunkPosition &pos);

  std::unique_ptr<ChunkRenderData>
  requestChunkRenderData(const ChunkPosition &pos);

  Chunk *loadOrGenerateChunk(const ChunkPosition &pos);

private:
  QMutex m_chunksMutex;
  std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> m_chunks;
};

#endif // WORLD_H