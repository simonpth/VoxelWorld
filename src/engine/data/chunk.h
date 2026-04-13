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

  const Block &block(int x, int y, int z) const{
    return blocks[blockIndex(x, y, z)];
  }

  void setBlock(int x, int y, int z, const Block &block) {
    blocks[blockIndex(x, y, z)] = block;
  }
};

struct ChunkPosition {
  int16_t x, y, z;

  constexpr ChunkPosition(int16_t x = 0, int16_t y = 0, int16_t z = 0) noexcept
      : x(x), y(y), z(z) {}

  constexpr bool operator==(const ChunkPosition &other) const noexcept {
    return x == other.x && y == other.y && z == other.z;
  }

  constexpr ChunkPosition operator+(const ChunkPosition &other) const noexcept {
    return ChunkPosition(x + other.x, y + other.y, z + other.z);
  }

  constexpr ChunkPosition operator-(const ChunkPosition &other) const noexcept {
    return ChunkPosition(x - other.x, y - other.y, z - other.z);
  }
};

namespace std {
template <> struct hash<ChunkPosition> {
  constexpr size_t operator()(const ChunkPosition &pos) const noexcept {
    return (static_cast<size_t>(pos.y) << 32) |
           (static_cast<size_t>(pos.z) << 16) | pos.x;
  }
};
} // namespace std

#endif // CHUNK_H