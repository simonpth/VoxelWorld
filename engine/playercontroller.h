#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QDebug>

#include <QObject>
#include <QVector3D>
#include <QtCore/qtmetamacros.h>
#include <QtGui/qvectornd.h>
#include <qqmlintegration.h>
#include <QMutex>

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
  int x;
  int y;
  int z;

  PlayerChunkPos(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}

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

  QVector3D position() const { return m_position; }
  PlayerChunkPos currentChunk() const { return m_currentChunk; }
  QVector3D rotation() const { return m_rotation; }

signals:
  void chunkChanged(PlayerChunkPos newChunk);

private:
  QVector3D m_position;
  PlayerChunkPos m_currentChunk = PlayerChunkPos(0, 0, 0);
  QVector3D m_velocity;
  QVector3D m_rotation; // pitch, yaw, roll

  PlayerControllerInput m_input;
  QMutex m_inputMutex;
};

#endif // PLAYERCONTROLLER_H