#ifndef PLAYERCHUNKPOS_H
#define PLAYERCHUNKPOS_H

#include "engine/settings.h"
#include <cstdint>

struct PlayerChunkPos {
  PlayerChunkPos(int16_t x = 0, int16_t y = 0, int16_t z = 0)
      : m_x(x), m_y(y), m_z(z) {
    if (m_x < 0)
      m_x += m_planetSizeInChunks; // Wrap around for negative coordinates
    if (m_z < 0)
      m_z += m_planetSizeInChunks; // Wrap around for negative coordinates

    m_x %= m_planetSizeInChunks; // Wrap around for positive coordinates
    m_z %= m_planetSizeInChunks; // Wrap around for positive coordinates
  }

  bool operator==(const PlayerChunkPos &other) const {
    return x() == other.x() && y() == other.y() && z() == other.z();
  }

  int16_t x() const { return m_x; }
  int16_t y() const { return m_y; }
  int16_t z() const { return m_z; }

  void addX(int16_t dx) {
    m_x += dx;
    if (m_x < 0)
      m_x += m_planetSizeInChunks; // Wrap around for negative coordinates
    m_x %= m_planetSizeInChunks; // Wrap around for positive coordinates
  }

  void addY(int16_t dy) {
    m_y += dy;
    // No wrapping in Y direction since it's vertical
  }

  void addZ(int16_t dz) {
    m_z += dz;
    if (m_z < 0)
      m_z += m_planetSizeInChunks; // Wrap around for negative coordinates
    m_z %= m_planetSizeInChunks; // Wrap around for positive coordinates
  }

private:
  int16_t m_x, m_y, m_z;
  int m_planetSizeInChunks = Settings::instance().planetSizeInChunks();
};

#endif // PLAYERCHUNKPOS_H