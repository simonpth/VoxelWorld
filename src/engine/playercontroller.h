#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <atomic>
#include <chrono>
#include <vector>
#include <shared_mutex>
#include <mutex>

#include <glm/glm.hpp>

#include "chunk.h"
#include "playerchunkpos.h"

struct PlayerControllerInput {
  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool jump = false;
};

class PlayerController {
public:
  PlayerController();

  void keyPressed(const int &key);
  void keyReleased(const int &key);
  void mouseMoved(const float &deltaX, const float &deltaY);

  void update();
  glm::vec3 calculateDirection();
  void move(glm::vec3 delta);

  glm::vec3 position() {
    std::shared_lock lock(m_positionMutex);
    return m_position;
  }
  void setPosition(int x, int y, int z) {
    std::unique_lock lock(m_positionMutex);
    m_position = glm::vec3(x%Chunk::SIZE, y%Chunk::SIZE, z%Chunk::SIZE);
    PlayerChunkPos newChunkPos = PlayerChunkPos(x/Chunk::SIZE, y/Chunk::SIZE, z/Chunk::SIZE);
    std::unique_lock chunkLock(m_chunkMutex);
    if(newChunkPos != m_currentChunk) {
      m_currentChunk = newChunkPos;
      m_chunksToRenderDirty.store(true);
    }
  }
  glm::vec3 worldPosition() {
    std::shared_lock lock(m_positionMutex);
    std::shared_lock chunkLock(m_chunkMutex);
    return glm::vec3(m_currentChunk.x * Chunk::SIZE + m_position.x,
                     m_currentChunk.y * Chunk::SIZE + m_position.y,
                     m_currentChunk.z * Chunk::SIZE + m_position.z);
  }
  PlayerChunkPos currentChunk() {
    std::shared_lock lock(m_chunkMutex);
    return m_currentChunk;
  }
  glm::vec3 rotation() {
    std::shared_lock lock(m_rotationMutex);
    return m_rotation;
  }

  int renderDistance() { return m_renderDistance.load(); }
  void setRenderDistance(int distance);
  bool chunksToRenderDirty() const { return m_chunksToRenderDirty.load(); }
  void setChunksToRenderDirty(bool dirty) {
    m_chunksToRenderDirty.store(dirty);
  }
  std::vector<ChunkPosition> relativeChunkOffsets() {
    std::shared_lock lock(m_relativeChunkOffsetsMutex);
    return m_relativeChunkOffsets;
  }

  glm::vec3 front() {
    std::shared_lock lock(m_rotationMutex);
    float pitch = glm::radians(m_rotation.x);
    float yaw = glm::radians(m_rotation.y);
    return glm::normalize(glm::vec3(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)
    ));
  }

  glm::vec3 up() {
    return glm::vec3(0, 1, 0);
  }

private:
  glm::vec3 m_position = glm::vec3(3.0f, 3.0f, 3.0f);
  std::shared_mutex m_positionMutex;
  PlayerChunkPos m_currentChunk = PlayerChunkPos(0, 0, 0);
  std::shared_mutex m_chunkMutex;
  glm::vec3 m_velocity;
  std::shared_mutex m_velocityMutex;
  glm::vec3 m_rotation; // pitch, yaw, roll
  std::shared_mutex m_rotationMutex;

  PlayerControllerInput m_input;
  std::shared_mutex m_inputMutex;

  std::chrono::steady_clock::time_point m_lastUpdateTime;
  std::mutex m_updateMutex;

  std::atomic<int> m_renderDistance;
  std::mutex m_renderDistanceSetMutex;
  std::vector<ChunkPosition> m_relativeChunkOffsets;
  std::shared_mutex m_relativeChunkOffsetsMutex;
  std::atomic<bool> m_chunksToRenderDirty = false;
  void calculateRelativeChunkOffsets();
};

#endif // PLAYERCONTROLLER_H