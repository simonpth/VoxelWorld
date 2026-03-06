#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QDebug>
#include <QMutex>
#include <QObject>
#include <QVector3D>
#include <QMutexLocker>
#include <qqmlintegration.h>

#include <chrono>

struct PlayerControllerInput {
  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool jump = false;
};

struct PlayerChunkPos {
  int16_t x, y, z;

  PlayerChunkPos(int16_t x = 0, int16_t y = 0, int16_t z = 0)
      : x(x), y(y), z(z) {}

  bool operator==(const PlayerChunkPos &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

static const float PLAYER_SPEED = 5.0f; // units per second

class PlayerController : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QVector3D position READ position)
  Q_PROPERTY(PlayerChunkPos currentChunk READ currentChunk)
  Q_PROPERTY(QVector3D rotation READ rotation)

public:
  PlayerController(QObject *parent = nullptr);

  Q_INVOKABLE void keyPressed(const int &key);
  Q_INVOKABLE void keyReleased(const int &key);
  Q_INVOKABLE void mouseMoved(const float &deltaX, const float &deltaY);

  void update(std::chrono::nanoseconds nsDelta);
  QVector3D calculateDirection();
  void move(QVector3D delta);

  QVector3D position() {
    QMutexLocker locker(&m_positionMutex);
    return m_position;
  }
  PlayerChunkPos currentChunk() {
    QMutexLocker locker(&m_chunkMutex);
    return m_currentChunk;
  }
  QVector3D rotation() {
    QMutexLocker locker(&m_rotationMutex);
    return m_rotation;
  }

signals:
  void chunkChanged(PlayerChunkPos newChunk);

private:
  QVector3D m_position;
  QMutex m_positionMutex;
  PlayerChunkPos m_currentChunk = PlayerChunkPos(0, 0, 0);
  QMutex m_chunkMutex;
  QVector3D m_velocity;
  QMutex m_velocityMutex;
  QVector3D m_rotation; // pitch, yaw, roll
  QMutex m_rotationMutex;

  PlayerControllerInput m_input;
  QMutex m_inputMutex;
};

#endif // PLAYERCONTROLLER_H