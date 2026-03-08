#include "playercontroller.h"

PlayerController::PlayerController(QObject *parent) : QObject(parent) {
  setRenderDistance(4); // default render distance
}

void PlayerController::keyPressed(const int &key) {
  QWriteLocker locker(&m_inputMutex);
  switch (key) {
  case Qt::Key_W:
    m_input.moveForward = true;
    break;
  case Qt::Key_S:
    m_input.moveBackward = true;
    break;
  case Qt::Key_A:
    m_input.moveLeft = true;
    break;
  case Qt::Key_D:
    m_input.moveRight = true;
    break;
  case Qt::Key_Space:
    m_input.jump = true;
    m_input.moveUp = true;
    break;
  case Qt::Key_Shift:
    m_input.moveDown = true;
    break;
  default:
    break;
  }
}

void PlayerController::keyReleased(const int &key) {
  QWriteLocker locker(&m_inputMutex);
  switch (key) {
  case Qt::Key_W:
    m_input.moveForward = false;
    break;
  case Qt::Key_S:
    m_input.moveBackward = false;
    break;
  case Qt::Key_A:
    m_input.moveLeft = false;
    break;
  case Qt::Key_D:
    m_input.moveRight = false;
    break;
  case Qt::Key_Space:
    m_input.jump = false;
    m_input.moveUp = false;
    break;
  case Qt::Key_Shift:
    m_input.moveDown = false;
    break;
  default:
    break;
  }
}

void PlayerController::mouseMoved(const float &deltaX, const float &deltaY) {
  QWriteLocker locker(&m_rotationMutex);
  float y = m_rotation.y() + deltaX * 0.1f; // Yaw
  if (y < 0)
    y += 360;
  if (y >= 360)
    y -= 360;
  m_rotation.setY(y);

  float x = m_rotation.x() + deltaY * 0.1f; // Pitch
  if (x < -89)
    x = -89;
  if (x > 89)
    x = 89;
  m_rotation.setX(x);
}

// The first call to this function will initialize the lastUpdateTime, so the
// player won't move until the second call
void PlayerController::update() {
  QMutexLocker updateLocker(&m_updateMutex);
  QWriteLocker locker(&m_velocityMutex);
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

QVector3D PlayerController::calculateDirection() {
  QVector3D direction;
  QReadLocker inputLocker(&m_inputMutex);

  if (m_input.moveForward)
    direction += QVector3D(0, 0, -1);
  if (m_input.moveBackward)
    direction += QVector3D(0, 0, 1);
  if (m_input.moveLeft)
    direction += QVector3D(-1, 0, 0);
  if (m_input.moveRight)
    direction += QVector3D(1, 0, 0);
  if (m_input.moveUp)
    direction += QVector3D(0, 1, 0);
  if (m_input.moveDown)
    direction += QVector3D(0, -1, 0);

  if (!direction.isNull()) {
    direction.normalize();

    float yaw;
    {
      QReadLocker rotationLocker(&m_rotationMutex);
      yaw = m_rotation.y();
    }
    float cosYaw = std::cos(qDegreesToRadians(yaw));
    float sinYaw = std::sin(qDegreesToRadians(yaw));

    // Rotate the direction vector by the yaw angle around the Y axis
    float rotatedX = direction.x() * cosYaw - direction.z() * sinYaw;
    float rotatedZ = direction.x() * sinYaw + direction.z() * cosYaw;
    direction.setX(rotatedX);
    direction.setZ(rotatedZ);
  }

  return direction;
}

void PlayerController::setRenderDistance(int distance) {
  QMutexLocker locker(&m_renderDistanceSetMutex);
  if (m_renderDistance != distance) {
    m_renderDistance.store(distance);
    calculateRelativeChunkOffsets();
    m_chunksToRenderDirty.store(true);
    emit renderDistanceChanged(distance);
  }
}

void PlayerController::calculateRelativeChunkOffsets() {
  QWriteLocker locker(&m_relativeChunkOffsetsMutex);
  m_relativeChunkOffsets.clear();
  m_relativeChunkOffsets.reserve((2 * m_renderDistance + 1) *
                                 (2 * m_renderDistance + 1) *
                                 (2 * m_renderDistance + 1));
  for (int x = -m_renderDistance; x <= m_renderDistance; ++x) {
    for (int y = -m_renderDistance; y <= m_renderDistance; ++y) {
      for (int z = -m_renderDistance; z <= m_renderDistance; ++z) {
        if (x * x + y * y + z * z < m_renderDistance * m_renderDistance)
          m_relativeChunkOffsets.emplace_back(x, y, z);
      }
    }
  }
  m_relativeChunkOffsets.shrink_to_fit();
}

void PlayerController::move(QVector3D delta) {
  QWriteLocker poslocker(&m_positionMutex);
  QWriteLocker chunklocker(&m_chunkMutex);
  m_position += delta;

  if (m_position.x() < 0) {
    m_position.setX(m_position.x() + Chunk::SIZE);
    m_currentChunk.x -= 1;
    m_chunksToRenderDirty.store(true);
  }
  if (m_position.x() >= Chunk::SIZE) {
    m_position.setX(m_position.x() - Chunk::SIZE);
    m_currentChunk.x += 1;
    m_chunksToRenderDirty.store(true);
  }

  if (m_position.y() < 0) {
    m_position.setY(m_position.y() + Chunk::SIZE);
    m_currentChunk.y -= 1;
    m_chunksToRenderDirty.store(true);
  }
  if (m_position.y() >= Chunk::SIZE) {
    m_position.setY(m_position.y() - Chunk::SIZE);
    m_currentChunk.y += 1;
    m_chunksToRenderDirty.store(true);
  }

  if (m_position.z() < 0) {
    m_position.setZ(m_position.z() + Chunk::SIZE);
    m_currentChunk.z -= 1;
    m_chunksToRenderDirty.store(true);
  }
  if (m_position.z() >= Chunk::SIZE) {
    m_position.setZ(m_position.z() - Chunk::SIZE);
    m_currentChunk.z += 1;
    m_chunksToRenderDirty.store(true);
  }
}
