#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include <cassert>
#include <cstdint>
#include <functional>

struct Chunk {
  static constexpr int SIZE = 32;
  static constexpr int AREA = SIZE * SIZE;
  static constexpr int VOLUME = SIZE * SIZE * SIZE;

  Block blocks[VOLUME]; // 32x32x32 blocks

  int blockIndex(int x, int y, int z) const noexcept {
    assert(x >= 0 && x < SIZE);
    assert(y >= 0 && y < SIZE);
    assert(z >= 0 && z < SIZE);
    return y * AREA + z * SIZE + x;
  }

  Block &block(int x, int y, int z) {
    return blocks[blockIndex(x, y, z)];
  }

  void setBlock(int x, int y, int z, const Block &block) {
    blocks[blockIndex(x, y, z)] = block;
  }
};

struct ChunkPosition {
  uint16_t x, y, z;

  ChunkPosition(uint16_t x = 0, uint16_t y = 0, uint16_t z = 0)
      : x(x), y(y), z(z) {}

  bool operator==(const ChunkPosition &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

namespace std {
template <> struct hash<ChunkPosition> {
  size_t operator()(const ChunkPosition &pos) const {
    return (static_cast<size_t>(pos.y) << 32) |
           (static_cast<size_t>(pos.z) << 16) | pos.x;
  }
};
} // namespace std

#endif // CHUNK_H