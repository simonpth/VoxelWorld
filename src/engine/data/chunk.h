#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include "engine/settings.h"

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>

#define CHUNK_SIZE 32
#define CHUNK_CHUNKPIECES 4

struct ChunkPieceData {
  static constexpr int SIZE = CHUNK_SIZE / CHUNK_CHUNKPIECES; // 8x8x8 blocks
  static constexpr int AREA = SIZE * SIZE;
  static constexpr int VOLUME = SIZE * SIZE * SIZE;

  Block blocks[VOLUME]; // 8x8x8 blocks

  std::pair<bool, Block> checkUniformity() const {
    const Block &firstBlock = blocks[0];
    for (int i = 1; i < VOLUME; ++i) {
      if (blocks[i].id != firstBlock.id || blocks[i].data != firstBlock.data) {
        return {false, Block()};
      }
    }
    return {true, firstBlock};
  }

  void setBlock(int x, int y, int z, const Block &block) {
    blocks[blockIndex(x, y, z)] = block;
  }

  Block block(int x, int y, int z) const {
    return blocks[blockIndex(x, y, z)];
  }

private:
  int blockIndex(int x, int y, int z) const noexcept {
    assert(x >= 0 && x < SIZE);
    assert(y >= 0 && y < SIZE);
    assert(z >= 0 && z < SIZE);
    return y * AREA + z * SIZE + x;
  }
};

struct ChunkPiece {
  std::unique_ptr<ChunkPieceData> data;
  Block unifiedBlock;

  // Default constructor
  ChunkPiece() = default;

  // Copy constructor
  ChunkPiece(const ChunkPiece &other)
      : unifiedBlock(other.unifiedBlock) {
    if (other.data) {
      data = std::make_unique<ChunkPieceData>(*other.data);
    }
  }

  // Copy assignment operator
  ChunkPiece &operator=(const ChunkPiece &other) {
    if (this != &other) {
      unifiedBlock = other.unifiedBlock;
      data = other.data ? std::make_unique<ChunkPieceData>(*other.data) : nullptr;
    }
    return *this;
  }

  // Move constructor and move assignment (keep efficient moves)
  ChunkPiece(ChunkPiece &&) = default;
  ChunkPiece &operator=(ChunkPiece &&) = default;

  void checkUniformity() {
    if (data) {
      auto [isUniform, block] = data->checkUniformity();
      if (isUniform) {
        unifiedBlock = block;
        data.reset(); // Free the detailed block data
      }
    }
  }

  void setBlock(int x, int y, int z, const Block &block) {
    if (data) {
      data->setBlock(x, y, z, block);
      checkUniformity();
    } else {
      if (block.id != unifiedBlock.id || block.data != unifiedBlock.data) {
        // Need to create detailed data
        data = std::make_unique<ChunkPieceData>();
        for (int i = 0; i < ChunkPieceData::VOLUME; ++i) {
          data->blocks[i] = unifiedBlock;
        }
        data->setBlock(x, y, z, block);
      }
    }
  }

  Block block(int x, int y, int z) const {
    if (data) {
      return data->block(x, y, z);
    } else {
      return unifiedBlock;
    }
  }
};

struct Chunk {
  static constexpr int CHUNKPIECES = CHUNK_CHUNKPIECES; // 4x4x4 pieces of 8x8x8 blocks
  static constexpr int SIZE = CHUNK_SIZE;
  static constexpr int AREA = SIZE * SIZE;
  static constexpr int VOLUME = SIZE * SIZE * SIZE;

  ChunkPiece pieces[CHUNKPIECES * CHUNKPIECES * CHUNKPIECES];

  void setBlock(int x, int y, int z, const Block &block) {
    int pieceX = x / ChunkPieceData::SIZE;
    int pieceY = y / ChunkPieceData::SIZE;
    int pieceZ = z / ChunkPieceData::SIZE;
    pieces[pieceIndex(pieceX, pieceY, pieceZ)].setBlock(x % ChunkPieceData::SIZE,
                                                        y % ChunkPieceData::SIZE,
                                                        z % ChunkPieceData::SIZE,
                                                        block);
  }

  Block block(int x, int y, int z) const {
    int pieceX = x / ChunkPieceData::SIZE;
    int pieceY = y / ChunkPieceData::SIZE;
    int pieceZ = z / ChunkPieceData::SIZE;
    return pieces[pieceIndex(pieceX, pieceY, pieceZ)].block(x % ChunkPieceData::SIZE,
                                                            y % ChunkPieceData::SIZE,
                                                            z % ChunkPieceData::SIZE);
  }

private:
  int pieceIndex(int x, int y, int z) const noexcept {
    assert(x >= 0 && x < CHUNKPIECES);
    assert(y >= 0 && y < CHUNKPIECES);
    assert(z >= 0 && z < CHUNKPIECES);
    return y * CHUNKPIECES * CHUNKPIECES + z * CHUNKPIECES + x;
  }
};

struct ChunkPosition {
  ChunkPosition(int16_t x = 0, int16_t y = 0, int16_t z = 0) noexcept
      : m_x(x), m_y(y), m_z(z) {
    if (this->m_x < 0)
      this->m_x += m_planetSizeInChunks; // Wrap around for negative coordinates
    if (this->m_z < 0)
      this->m_z += m_planetSizeInChunks; // Wrap around for negative coordinates

    this->m_x %= m_planetSizeInChunks; // Wrap around for positive coordinates
    this->m_z %= m_planetSizeInChunks; // Wrap around for positive coordinates
  }

  bool operator==(const ChunkPosition &other) const noexcept {
    return x() == other.x() && y() == other.y() && z() == other.z();
  }

  ChunkPosition operator+(const ChunkPosition &other) const noexcept {
    return ChunkPosition(x() + other.x(), y() + other.y(), z() + other.z());
  }

  ChunkPosition operator-(const ChunkPosition &other) const noexcept {
    return ChunkPosition(x() - other.x(), y() - other.y(), z() - other.z());
  }

  constexpr int16_t x() const noexcept { return m_x; }
  constexpr int16_t y() const noexcept { return m_y; }
  constexpr int16_t z() const noexcept { return m_z; }

private:
  int16_t m_x, m_y, m_z;
  int m_planetSizeInChunks = Settings::instance().planetSizeInChunks(); // For world wrapping
};

struct ChunkPositionOffset {
  int16_t x, y, z;

  constexpr ChunkPositionOffset(int16_t x = 0, int16_t y = 0, int16_t z = 0) noexcept
      : x(x), y(y), z(z) {}
};

namespace std {
template <>
struct hash<ChunkPosition> {
  size_t operator()(const ChunkPosition &pos) const noexcept {
    uint64_t key = (uint64_t)(uint16_t)pos.x() | ((uint64_t)(uint16_t)pos.y() << 16) | ((uint64_t)(uint16_t)pos.z() << 32);

    // Murmur-inspired mix
    key = (~key) + (key << 21);
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 28);
    key = key + (key << 31);

    return (size_t)key;
  }
};
} // namespace std

#endif // CHUNK_H