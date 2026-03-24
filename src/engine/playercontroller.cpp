#include "playercontroller.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "constants.h"

PlayerController::PlayerController() {
  setRenderDistance(10); // default render distance
  setPosition(16, 120, 16);
}

void PlayerController::keyPressed(const int &key) {
  std::unique_lock lock(m_inputMutex);
  switch (key) {
  case Constants::KEY_W:
    m_input.moveForward = true;
    break;
  case Constants::KEY_S:
    m_input.moveBackward = true;
    break;
  case Constants::KEY_A:
    m_input.moveLeft = true;
    break;
  case Constants::KEY_D:
    m_input.moveRight = true;
    break;
  case Constants::KEY_SPACE:
    m_input.jump = true;
    m_input.moveUp = true;
    break;
  case Constants::KEY_SHIFT:
    m_input.moveDown = true;
    break;
  default:
    break;
  }
}

void PlayerController::keyReleased(const int &key) {
  std::unique_lock lock(m_inputMutex);
  switch (key) {
  case Constants::KEY_W:
    m_input.moveForward = false;
    break;
  case Constants::KEY_S:
    m_input.moveBackward = false;
    break;
  case Constants::KEY_A:
    m_input.moveLeft = false;
    break;
  case Constants::KEY_D:
    m_input.moveRight = false;
    break;
  case Constants::KEY_SPACE:
    m_input.jump = false;
    m_input.moveUp = false;
    break;
  case Constants::KEY_SHIFT:
    m_input.moveDown = false;
    break;
  default:
    break;
  }
}

void PlayerController::mouseMoved(const float &deltaX, const float &deltaY) {
  std::unique_lock lock(m_rotationMutex);
  float y = m_rotation.y + deltaX * 0.1f; // Yaw
  if (y < 0)
    y += 360;
  if (y >= 360)
    y -= 360;
  m_rotation.y = y;

  float x = m_rotation.x + deltaY * 0.1f; // Pitch
  if (x < -89)
    x = -89;
  if (x > 89)
    x = 89;
  m_rotation.x = x;
}

// The first call to this function will initialize the lastUpdateTime, so the
// player won't move until the second call
void PlayerController::update() {
  std::unique_lock updateLock(m_updateMutex);
  std::unique_lock velocityLock(m_velocityMutex);
  auto now = std::chrono::steady_clock::now();
  if (m_lastUpdateTime.time_since_epoch().count() == 0) {
    m_lastUpdateTime = now;
    return;
  }
  auto nsDelta = std::chrono::duration_cast<std::chrono::nanoseconds>(
      now - m_lastUpdateTime);
  m_lastUpdateTime = now;
  m_velocity = calculateDirection() * PLAYER_SPEED;
  move(m_velocity * (nsDelta.count() / 1e9f));
}

glm::vec3 PlayerController::calculateDirection() {
  glm::vec3 direction(0.0f, 0.0f, 0.0f);
  std::shared_lock inputLock(m_inputMutex);

  if (m_input.moveForward)
    direction += glm::vec3(0, 0, -1);
  if (m_input.moveBackward)
    direction += glm::vec3(0, 0, 1);
  if (m_input.moveLeft)
    direction += glm::vec3(-1, 0, 0);
  if (m_input.moveRight)
    direction += glm::vec3(1, 0, 0);
  if (m_input.moveUp)
    direction += glm::vec3(0, 1, 0);
  if (m_input.moveDown)
    direction += glm::vec3(0, -1, 0);

  if (direction.x != 0 || direction.y != 0 || direction.z != 0) {
    direction = glm::normalize(direction);

    float yaw;
    {
      std::shared_lock lock(m_rotationMutex);
      yaw = m_rotation.y;
    }
    float cosYaw = std::cos(glm::radians(yaw));
    float sinYaw = std::sin(glm::radians(yaw));

    // Rotate the direction vector by the yaw angle around the Y axis
    float rotatedX = direction.x * cosYaw - direction.z * sinYaw;
    float rotatedZ = direction.x * sinYaw + direction.z * cosYaw;
    direction.x = rotatedX;
    direction.z = rotatedZ;
  }

  return direction;
}

void PlayerController::setRenderDistance(int distance) {
  std::unique_lock lock(m_renderDistanceSetMutex);
  if (m_renderDistance != distance) {
    m_renderDistance.store(distance);
    calculateRelativeChunkOffsets();
    m_chunksToRenderDirty.store(true);
  }
}

void PlayerController::calculateRelativeChunkOffsets() {
  std::unique_lock lock(m_relativeChunkOffsetsMutex);
  m_relativeChunkOffsets.clear();
  m_relativeChunkOffsets.reserve((2 * m_renderDistance + 1) *
                                 (2 * m_renderDistance + 1));
  for (int x = -m_renderDistance; x <= m_renderDistance; ++x) {
    for (int z = -m_renderDistance; z <= m_renderDistance; ++z) {
      if (x * x + z * z < m_renderDistance * m_renderDistance)
        m_relativeChunkOffsets.emplace_back(x, 0, z);
    }
  }
  m_relativeChunkOffsets.shrink_to_fit();
}

void PlayerController::move(glm::vec3 delta) {
  std::unique_lock positionLock(m_positionMutex);
  std::unique_lock chunkLock(m_chunkMutex);
  m_position += delta;

  if (m_position.x < 0) {
    m_position.x = m_position.x + Chunk::SIZE;
    m_currentChunk.x -= 1;
    m_chunksToRenderDirty.store(true);
  }
  if (m_position.x >= Chunk::SIZE) {
    m_position.x = m_position.x - Chunk::SIZE;
    m_currentChunk.x += 1;
    m_chunksToRenderDirty.store(true);
  }

  if (m_position.y < 0) {
    m_position.y = m_position.y + Chunk::SIZE;
    m_currentChunk.y -= 1;
    m_chunksToRenderDirty.store(true);
  }
  if (m_position.y >= Chunk::SIZE) {
    m_position.y = m_position.y - Chunk::SIZE;
    m_currentChunk.y += 1;
    m_chunksToRenderDirty.store(true);
  }

  if (m_position.z < 0) {
    m_position.z = m_position.z + Chunk::SIZE;
    m_currentChunk.z -= 1;
    m_chunksToRenderDirty.store(true);
  }
  if (m_position.z >= Chunk::SIZE) {
    m_position.z = m_position.z - Chunk::SIZE;
    m_currentChunk.z += 1;
    m_chunksToRenderDirty.store(true);
  }
}
