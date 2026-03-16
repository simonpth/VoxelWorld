#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QtCore/qreadwritelock.h>
#include <QtGui/qvectornd.h>
#include <atomic>
#include <chrono>
#include <vector>

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QReadWriteLock>
#include <QVector3D>
#include <qqmlintegration.h>

#include "chunk.h"

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

static const float PLAYER_SPEED = 20.0f; // units per second

class PlayerController : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QVector3D position READ worldPosition)
  Q_PROPERTY(PlayerChunkPos currentChunk READ currentChunk)
  Q_PROPERTY(QVector3D rotation READ rotation)
  Q_PROPERTY(int renderDistance READ renderDistance WRITE setRenderDistance
                 NOTIFY renderDistanceChanged)

public:
  PlayerController(QObject *parent = nullptr);

  Q_INVOKABLE void keyPressed(const int &key);
  Q_INVOKABLE void keyReleased(const int &key);
  Q_INVOKABLE void mouseMoved(const float &deltaX, const float &deltaY);

  void update();
  QVector3D calculateDirection();
  void move(QVector3D delta);

  QVector3D position() {
    QReadLocker locker(&m_positionMutex);
    return m_position;
  }
  void setPosition(int x, int y, int z) {
    QWriteLocker locker(&m_positionMutex);
    m_position = QVector3D(x%Chunk::SIZE, y%Chunk::SIZE, z%Chunk::SIZE);
    PlayerChunkPos newChunkPos = PlayerChunkPos(x/Chunk::SIZE, y/Chunk::SIZE, z/Chunk::SIZE);
    QWriteLocker chunkLocker(&m_chunkMutex);
    if(newChunkPos != m_currentChunk) {
      m_currentChunk = newChunkPos;
      m_chunksToRenderDirty.store(true);
    }
  }
  QVector3D worldPosition() {
    QReadLocker locker(&m_positionMutex);
    QReadLocker chunkLocker(&m_chunkMutex);
    return QVector3D(m_currentChunk.x * Chunk::SIZE + m_position.x(),
                     m_currentChunk.y * Chunk::SIZE + m_position.y(),
                     m_currentChunk.z * Chunk::SIZE + m_position.z());
  }
  PlayerChunkPos currentChunk() {
    QReadLocker locker(&m_chunkMutex);
    return m_currentChunk;
  }
  QVector3D rotation() {
    QReadLocker locker(&m_rotationMutex);
    return m_rotation;
  }

  int renderDistance() { return m_renderDistance.load(); }
  void setRenderDistance(int distance);
  bool chunksToRenderDirty() const { return m_chunksToRenderDirty.load(); }
  void setChunksToRenderDirty(bool dirty) {
    m_chunksToRenderDirty.store(dirty);
  }
  std::vector<ChunkPosition> relativeChunkOffsets() {
    QReadLocker locker(&m_relativeChunkOffsetsMutex);
    return m_relativeChunkOffsets;
  }

signals:
  void renderDistanceChanged(int distance);

private:
  QVector3D m_position = QVector3D(3.0f, 3.0f, 3.0f);
  QReadWriteLock m_positionMutex;
  PlayerChunkPos m_currentChunk = PlayerChunkPos(0, 0, 0);
  QReadWriteLock m_chunkMutex;
  QVector3D m_velocity;
  QReadWriteLock m_velocityMutex;
  QVector3D m_rotation; // pitch, yaw, roll
  QReadWriteLock m_rotationMutex;

  PlayerControllerInput m_input;
  QReadWriteLock m_inputMutex;

  std::chrono::steady_clock::time_point m_lastUpdateTime;
  QMutex m_updateMutex;

  std::atomic<int> m_renderDistance;
  QMutex m_renderDistanceSetMutex;
  std::vector<ChunkPosition> m_relativeChunkOffsets;
  QReadWriteLock m_relativeChunkOffsetsMutex;
  std::atomic<bool> m_chunksToRenderDirty = false;
  void calculateRelativeChunkOffsets();
};

#endif // PLAYERCONTROLLER_H