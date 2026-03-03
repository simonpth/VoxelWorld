#include "playercontroller.h"
#include <QtCore/qnamespace.h>

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
  m_rotation.setY(m_rotation.y() + deltaX * 0.1f); // Yaw
  m_rotation.setX(m_rotation.x() + deltaY * 0.1f); // Pitch
}