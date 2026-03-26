#ifndef RENDERPLAYERCONTROLLER_H
#define RENDERPLAYERCONTROLLER_H

#include <chrono>

#include <glm/glm.hpp>
#include "../data/chunk.h"
#include "playercontrollerinterface.h"

struct RenderPlayerControllerInputState
{
  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool jump = false;
};

class RenderPlayerController : public PlayerControllerInterface
{
public:
  RenderPlayerController()
  {
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
    m_currentChunk = PlayerChunkPos(0, 8, 0);

    m_front = glm::vec3(0, 0, -1);
    m_up = glm::vec3(0, 1, 0);
  }

  glm::vec3 position() const override { return m_position; }
  PlayerChunkPos currentChunk() const override { return m_currentChunk; }
  glm::vec3 rotation() const override { return m_rotation; }
  glm::vec3 front() const override { return m_front; }
  glm::vec3 up() const override { return m_up; }
  glm::vec3 worldPosition() const override
  {
    return glm::vec3(m_currentChunk.x * Chunk::SIZE + m_position.x,
                     m_currentChunk.y * Chunk::SIZE + m_position.y,
                     m_currentChunk.z * Chunk::SIZE + m_position.z);
  }

  void addRotation(const glm::vec3 &rotation) override
  {
    m_rotation += rotation;

    if (m_rotation.x > 89.0f)
      m_rotation.x = 89.0f;
    if (m_rotation.x < -89.0f)
      m_rotation.x = -89.0f;

    // Update front and up vectors based on the new rotation
    float pitch = glm::radians(m_rotation.x);
    float yaw = glm::radians(m_rotation.y);

    m_front.x = cos(pitch) * cos(yaw);
    m_front.y = sin(pitch);
    m_front.z = cos(pitch) * sin(yaw);
    m_front = glm::normalize(m_front);

    // Recalculate the right and up vectors
    glm::vec3 right = glm::normalize(glm::cross(m_front, glm::vec3(0, 1, 0)));
    m_up = glm::normalize(glm::cross(right, m_front));
  }

  void update() override
  {
    if (m_firstUpdate)
    {
      m_lastUpdateTime = std::chrono::steady_clock::now();
      m_firstUpdate = false;
      return;
    }
    auto now = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastUpdateTime);
    m_lastUpdateTime = now;

    glm::vec3 direction(0.0f, 0.0f, 0.0f);
    glm::vec3 horizontalFront = glm::vec3(m_front.x, 0.0f, m_front.z);
    glm::vec3 horizontalRight = glm::vec3(-m_front.z, 0.0f, m_front.x);

    if (m_inputState.moveForward)
      direction += horizontalFront;
    if (m_inputState.moveBackward)
      direction -= horizontalFront;
    if (m_inputState.moveLeft)
      direction -= horizontalRight;
    if (m_inputState.moveRight)
      direction += horizontalRight;
    if (m_inputState.moveUp)
      direction += glm::vec3(0, 1, 0);
    if (m_inputState.moveDown)
      direction -= glm::vec3(0, 1, 0);

    if (direction.x != 0 || direction.y != 0 || direction.z != 0)
    {
      direction = glm::normalize(direction);

      m_position += direction * PLAYER_SPEED * (deltaTime.count() / 1e9f);

      // Handle chunk transitions
      if (m_position.x < 0)
      {
        m_position.x += Chunk::SIZE;
        m_currentChunk.x -= 1;
        m_chunkChanged.store(true);
      }
      else if (m_position.x >= Chunk::SIZE)
      {
        m_position.x -= Chunk::SIZE;
        m_currentChunk.x += 1;
        m_chunkChanged.store(true);
      }
      if (m_position.y < 0)
      {
        m_position.y += Chunk::SIZE;
        m_currentChunk.y -= 1;
        m_chunkChanged.store(true);
      }
      else if (m_position.y >= Chunk::SIZE)
      {
        m_position.y -= Chunk::SIZE;
        m_currentChunk.y += 1;
        m_chunkChanged.store(true);
      }
      if (m_position.z < 0)
      {
        m_position.z += Chunk::SIZE;
        m_currentChunk.z -= 1;
        m_chunkChanged.store(true);
      }
      else if (m_position.z >= Chunk::SIZE)
      {
        m_position.z -= Chunk::SIZE;
        m_currentChunk.z += 1;
        m_chunkChanged.store(true);
      }
    }
  }

private:
  PlayerChunkPos m_currentChunk;
  glm::vec3 m_position;
  glm::vec3 m_rotation; // pitch, yaw, roll
  glm::vec3 m_front;
  glm::vec3 m_up;

  std::chrono::steady_clock::time_point m_lastUpdateTime;
  bool m_firstUpdate = true;
};

#endif // RENDERPLAYERCONTROLLER_H