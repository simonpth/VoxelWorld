#ifndef RENDERPLAYERCONTROLLER_H
#define RENDERPLAYERCONTROLLER_H

#include <atomic>
#include <chrono>
#include <mutex>

#include "engine/data/chunk.h"
#include "playercontrollerinterface.h"
#include <glm/glm.hpp>

#include "engine/settings.h"

class RenderPlayerController : public PlayerControllerInterface {
public:
  RenderPlayerController() {
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
    m_currentChunk = PlayerChunkPos(0, 4, 0);

    m_front = glm::vec3(0, 0, -1);
    m_up = glm::vec3(0, 1, 0);
  }

  glm::vec3 position() const override {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_position;
  }

  PlayerChunkPos currentChunk() const override {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_currentChunk;
  }

  glm::vec3 rotation() const override {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_rotation;
  }

  glm::vec3 front() const override {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_front;
  }

  glm::vec3 up() const override {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_up;
  }

  glm::vec3 worldPosition() const override {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return glm::vec3(m_currentChunk.x() * Chunk::SIZE + m_position.x,
                     m_currentChunk.y() * Chunk::SIZE + m_position.y,
                     m_currentChunk.z() * Chunk::SIZE + m_position.z);
  }

  void setInputState(const PlayerControllerInput &state) override {
    std::lock_guard<std::mutex> lock(m_inputMutex);
    m_inputState = state;
  }

  void addRotation(const glm::vec3 &rotation) override {
    std::lock_guard<std::mutex> lock(m_stateMutex);

    m_rotation += rotation;

    if (m_rotation.x > 89.0f)
      m_rotation.x = 89.0f;
    if (m_rotation.x < -89.0f)
      m_rotation.x = -89.0f;

    float pitch = glm::radians(m_rotation.x);
    float yaw = glm::radians(m_rotation.y);

    m_front.x = cos(pitch) * cos(yaw);
    m_front.y = sin(pitch);
    m_front.z = cos(pitch) * sin(yaw);
    m_front = glm::normalize(m_front);

    glm::vec3 right = glm::normalize(glm::cross(m_front, glm::vec3(0, 1, 0)));
    m_up = glm::normalize(glm::cross(right, m_front));
  }

  void update() override {
    if (m_firstUpdate) {
      m_lastUpdateTime = std::chrono::steady_clock::now();
      m_firstUpdate = false;
      return;
    }

    auto now = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastUpdateTime);
    m_lastUpdateTime = now;

    // Snapshot input state to minimize lock contention
    PlayerControllerInput input;
    {
      std::lock_guard<std::mutex> lock(m_inputMutex);
      input = m_inputState;
    }

    glm::vec3 direction(0.0f);
    glm::vec3 front, position;
    PlayerChunkPos currentChunk;

    // Snapshot mutable state
    {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      front = m_front;
      position = m_position;
      currentChunk = m_currentChunk;
    }

    glm::vec3 horizontalFront = glm::vec3(front.x, 0.0f, front.z);
    glm::vec3 horizontalRight = glm::vec3(-front.z, 0.0f, front.x);

    if (input.moveForward)
      direction += horizontalFront;
    if (input.moveBackward)
      direction -= horizontalFront;
    if (input.moveLeft)
      direction -= horizontalRight;
    if (input.moveRight)
      direction += horizontalRight;
    if (input.moveUp)
      direction += glm::vec3(0, 1, 0);
    if (input.moveDown)
      direction -= glm::vec3(0, 1, 0);

    if (direction.x != 0 || direction.y != 0 || direction.z != 0) {
      direction = glm::normalize(direction);
      position += direction * Settings::instance().playerSpeed() * (deltaTime.count() / 1e9f);

      bool chunkChanged = false;

      if (position.x < 0) {
        position.x += Chunk::SIZE;
        currentChunk.addX(-1);
        chunkChanged = true;
      } else if (position.x >= Chunk::SIZE) {
        position.x -= Chunk::SIZE;
        currentChunk.addX(1);
        chunkChanged = true;
      }
      if (position.y < 0) {
        position.y += Chunk::SIZE;
        currentChunk.addY(-1);
        chunkChanged = true;
      } else if (position.y >= Chunk::SIZE) {
        position.y -= Chunk::SIZE;
        currentChunk.addY(1);
        chunkChanged = true;
      }
      if (position.z < 0) {
        position.z += Chunk::SIZE;
        currentChunk.addZ(-1);
        chunkChanged = true;
      } else if (position.z >= Chunk::SIZE) {
        position.z -= Chunk::SIZE;
        currentChunk.addZ(1);
        chunkChanged = true;
      }

      // Write updated state back atomically
      {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_position = position;
        m_currentChunk = currentChunk;
      }

      if (chunkChanged)
        m_chunkChanged.store(true);
    }
  }

private:
  // Guarded by m_stateMutex
  PlayerChunkPos m_currentChunk;
  glm::vec3 m_position;
  glm::vec3 m_rotation;
  glm::vec3 m_front;
  glm::vec3 m_up;

  // Guarded by m_inputMutex
  PlayerControllerInput m_inputState;

  mutable std::mutex m_stateMutex;
  mutable std::mutex m_inputMutex;

  // Update-thread-only — no mutex needed
  std::chrono::steady_clock::time_point m_lastUpdateTime;
  bool m_firstUpdate = true;
};

#endif // RENDERPLAYERCONTROLLER_H