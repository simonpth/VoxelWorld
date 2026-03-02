#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

struct Block {
  uint16_t id; // blocks from 0 to 65535; blocks > 32768 are non solid blocks =>
               // easy bitwise check for solidity
  uint16_t data; // first 3 bits for rotation, last 5 bits for block state

  Block(uint16_t id = 0, uint16_t data = 0) : id(id), data(data) {}

  // Get the block state (last 5 bits)
  uint8_t state() const {
    return data & 0x1F; // 0x1F = 00011111 in binary
  }
  // Get the block rotation (first 3 bits)
  uint8_t rotation() const {
    return (data >> 5) &
           0x07; // Shift right by 5 and mask with 0x07 (00000111 in binary)
  }

  static constexpr uint16_t NON_SOLID_MASK = 0x8000;
  bool isSolid() const noexcept { return (id & NON_SOLID_MASK) == 0; }
};

#endif // BLOCK_H