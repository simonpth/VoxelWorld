#include "player.h"
#include "constants.h"

Player::Player() : Object() {
  // Initialize player-specific properties if needed
}

void Player::keyPress(const QString &key) {
  if (key == "W")
    m_keys.forward = true;
  else if (key == "S")
    m_keys.backward = true;
  else if (key == "A")
    m_keys.left = true;
  else if (key == "D")
    m_keys.right = true;
  else if (key == "Space")
    m_keys.jump = true;
  else if (key == "Shift")
    m_keys.sprint = true;
  else if (key == "Ctrl")
    m_keys.sneak = true;
}

void Player::keyRelease(const QString &key) {
  if (key == "W")
    m_keys.forward = false;
  else if (key == "S")
    m_keys.backward = false;
  else if (key == "A")
    m_keys.left = false;
  else if (key == "D")
    m_keys.right = false;
  else if (key == "Space")
    m_keys.jump = false;
  else if (key == "Shift")
    m_keys.sprint = false;
  else if (key == "Ctrl")
    m_keys.sneak = false;
}

void Player::tick() {
  move(TICK_INTERVAL_MS);
}

void Player::move(float ms) {
  float currentSpeed = SPEED * (ms / 1000.0f); // Convert ms to seconds
  if (m_keys.sprint)
    currentSpeed *= SPRINT_MULTIPLIER;
  if (m_keys.sneak)
    currentSpeed *= SNEAK_MULTIPLIER;

  if (m_keys.forward)
    m_position += QVector3D(m_facingDirection.x(), 0, m_facingDirection.z()) *
                  currentSpeed;
  if (m_keys.backward)
    m_position -= QVector3D(m_facingDirection.x(), 0, m_facingDirection.z()) *
                  currentSpeed;

  if (m_keys.left)
    m_position -=
        QVector3D::crossProduct(m_facingDirection, QVector3D(0, 1, 0)) *
        currentSpeed;

  if (m_keys.right)
    m_position +=
        QVector3D::crossProduct(m_facingDirection, QVector3D(0, 1, 0)) *
        currentSpeed;

  if (m_keys.jump)
    m_position.setY(m_position.y() + currentSpeed);
  if (m_keys.sneak)
    m_position.setY(m_position.y() - currentSpeed);
}