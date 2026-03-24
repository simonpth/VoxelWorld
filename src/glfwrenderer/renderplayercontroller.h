#ifndef RENDERPLAYERCONTROLLER_H
#define RENDERPLAYERCONTROLLER_H

#include <chrono>

#include <glm/glm.hpp>
#include "engine/chunk.h"
#include "engine/playerchunkpos.h"

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

class RenderPlayerController
{
public:
  RenderPlayerController()
  {
    setRenderDistance(4); // default render distance
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_currentChunk = PlayerChunkPos(0, 0, 0);
  }

  glm::vec3 position() const { return m_position; }
  PlayerChunkPos currentChunk() const { return m_currentChunk; }
  glm::vec3 rotation() const { return m_rotation; }

  glm::vec3 worldPosition()
  {
    return glm::vec3(m_currentChunk.x * Chunk::SIZE + m_position.x,
                     m_currentChunk.y * Chunk::SIZE + m_position.y,
                     m_currentChunk.z * Chunk::SIZE + m_position.z);
  }

  // deltaTime is in nanoseconds
  void update(const RenderPlayerControllerInputState &inputState, std::chrono::nanoseconds deltaTime)
  {
    glm::vec3 direction(0.0f, 0.0f, 0.0f);

    if (inputState.moveForward)
      direction += glm::vec3(0, 0, -1);
    if (inputState.moveBackward)
      direction += glm::vec3(0, 0, 1);
    if (inputState.moveLeft)
      direction += glm::vec3(-1, 0, 0);
    if (inputState.moveRight)
      direction += glm::vec3(1, 0, 0);
    if (inputState.moveUp)
      direction += glm::vec3(0, 1, 0);
    if (inputState.moveDown)
      direction += glm::vec3(0, -1, 0);

    if (direction.x != 0 || direction.y != 0 || direction.z != 0)
    {
      direction = glm::normalize(direction);

      float yaw = m_rotation.y;
      float cosYaw = std::cos(glm::radians(yaw));
      float sinYaw = std::sin(glm::radians(yaw));

      // Rotate the direction vector by the yaw angle around the Y axis
      float rotatedX = direction.x * cosYaw - direction.z * sinYaw;
      float rotatedZ = direction.x * sinYaw + direction.z * cosYaw;
      direction.x = rotatedX;
      direction.z = rotatedZ;

      m_position += direction * PLAYER_SPEED * (deltaTime.count() / 1e9f);

      if (m_position.x < 0)
      {
        m_position.x += Chunk::SIZE;
        m_chunksToRenderDirty = true;
      }
      else if (m_position.x >= Chunk::SIZE)
      {
        m_position.x -= Chunk::SIZE;
        m_chunksToRenderDirty = true;
      }

      if (m_position.y < 0)
      {
        m_position.y += Chunk::SIZE;
        m_chunksToRenderDirty = true;
      }
      else if (m_position.y >= Chunk::SIZE)
      {
        m_position.y -= Chunk::SIZE;
        m_chunksToRenderDirty = true;
      }

      if (m_position.z < 0)
      {
        m_position.z += Chunk::SIZE;
        m_chunksToRenderDirty = true;
      }
      else if (m_position.z >= Chunk::SIZE)
      {
        m_position.z -= Chunk::SIZE;
        m_chunksToRenderDirty = true;
      }
    }
  }

  void setRenderDistance(int distance)
  {
    m_relativeChunkOffsets.clear();
    m_relativeChunkOffsets.reserve((2 * distance + 1) *
                                   (2 * distance + 1));
    for (int x = -distance; x <= distance; ++x)
    {
      for (int z = -distance; z <= distance; ++z)
      {
        if (x * x + z * z < distance * distance)
          m_relativeChunkOffsets.emplace_back(x, 0, z);
      }
    }
    m_relativeChunkOffsets.shrink_to_fit();
    m_chunksToRenderDirty = true;
  }

  std::vector<ChunkPosition> relativeChunkOffsets() const { return m_relativeChunkOffsets; }

  glm::vec3 front() const
  {
    float pitch = glm::radians(m_rotation.x);
    float yaw = glm::radians(m_rotation.y);
    return glm::normalize(glm::vec3(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)));
  }

  glm::vec3 up() const
  {
    return glm::vec3(0, 1, 0);
  }

  bool chunksToRenderDirty() const { return m_chunksToRenderDirty; }
  void setChunksToRenderDirty(bool dirty) { m_chunksToRenderDirty = dirty; }

private:
  PlayerChunkPos m_currentChunk;
  glm::vec3 m_position;
  glm::vec3 m_rotation; // pitch, yaw, roll

  std::vector<ChunkPosition> m_relativeChunkOffsets;
  bool m_chunksToRenderDirty = false;
};

#endif // RENDERPLAYERCONTROLLER_H