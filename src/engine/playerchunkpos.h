#ifndef PLAYERCHUNKPOS_H
#define PLAYERCHUNKPOS_H

#include <cstdint>

struct PlayerChunkPos {
  int16_t x, y, z;

  PlayerChunkPos(int16_t x = 0, int16_t y = 0, int16_t z = 0)
      : x(x), y(y), z(z) {}

  bool operator==(const PlayerChunkPos &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

static const float PLAYER_SPEED = 20.0f; // units per second

#endif // PLAYERCHUNKPOS_H