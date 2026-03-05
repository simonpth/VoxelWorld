#include "playercontroller.h"
#include "chunk.h"
#include <QTransform>
#include <chrono>

PlayerController::PlayerController(QObject *parent) : QObject(parent) {}

void PlayerController::keyPressed(const int &key) {
  QMutexLocker locker(&m_inputMutex);
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
  QMutexLocker locker(&m_inputMutex);
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
  float y = m_rotation.y() + deltaX * 0.1f; // Yaw
  if (y < 0) y += 360;
  if (y >= 360) y -= 360;
  m_rotation.setY(y);
  
  float x = m_rotation.x() + deltaY * 0.1f; // Pitch
  if (x < -89) x = -89;
  if (x > 89) x = 89;
  m_rotation.setX(x);
}

void PlayerController::update(std::chrono::nanoseconds nsDelta) {
  m_velocity = calculateDirection() * PLAYER_SPEED;
  move(m_velocity * (nsDelta.count() / 1e9f));
}

QVector3D PlayerController::calculateDirection() {
  QVector3D direction;
  QMutexLocker locker(&m_inputMutex);

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

    QTransform t;
    t.rotate(m_rotation.y());
    QPointF inputFlat(direction.x(), direction.z());
    QPointF rotatedFlat = t.map(inputFlat);

    direction.setX(rotatedFlat.x());
    direction.setZ(rotatedFlat.y());
  }

  return direction;
}

void PlayerController::move(QVector3D delta) {
  m_position += delta;

  if (m_position.x() < 0) {
    m_position.setX(m_position.x() + Chunk::SIZE);
    m_currentChunk.x -= 1;
    emit chunkChanged(m_currentChunk);
  }
  if (m_position.x() >= Chunk::SIZE) {
    m_position.setX(m_position.x() - Chunk::SIZE);
    m_currentChunk.x += 1;
    emit chunkChanged(m_currentChunk);
  }

  if (m_position.y() < 0) {
    m_position.setY(m_position.y() + Chunk::SIZE);
    m_currentChunk.y -= 1;
    emit chunkChanged(m_currentChunk);
  }
  if (m_position.y() >= Chunk::SIZE) {
    m_position.setY(m_position.y() - Chunk::SIZE);
    m_currentChunk.y += 1;
    emit chunkChanged(m_currentChunk);
  }

  if (m_position.z() < 0) {
    m_position.setZ(m_position.z() + Chunk::SIZE);
    m_currentChunk.z -= 1;
    emit chunkChanged(m_currentChunk);
  }
  if (m_position.z() >= Chunk::SIZE) {
    m_position.setZ(m_position.z() - Chunk::SIZE);
    m_currentChunk.z += 1;
    emit chunkChanged(m_currentChunk);
  }
}