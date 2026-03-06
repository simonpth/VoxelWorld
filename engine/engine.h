#ifndef ENGINE_H
#define ENGINE_H

#include "chunk.h"
#include "objectengine.h"
#include "playercontroller.h"
#include "world.h"

#include <QObject>
#include <QThread>
#include <atomic>
#include <memory>
#include <qqmlintegration.h>
#include <vector>

class Engine : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(PlayerController *playerController READ playerController CONSTANT)

public:
  Engine(QObject *parent = nullptr);
  ~Engine();

  World *world() { return m_world.get(); }
  ObjectEngine *objectEngine() { return m_objectEngine.get(); }
  PlayerController *playerController() { return m_playerController.get(); }

  void setRenderDistance(int distance) {
    if (m_renderDistance != distance) {
      m_renderDistance = distance;
      calculateRelativeChunkOffsets();
      updateChunksToRender();
    }
  }
  int renderDistance() const { return m_renderDistance; }

  bool chunksToRenderDirty() const { return m_chunksToRenderDirty.load(); }

public slots:
  void run();
  void stop() { m_running = false; }

private slots:
  void onPlayerChunkChanged() {
    updateChunksToRender();
  };

private:
  QThread *m_gameLoopThread;
  std::atomic<bool> m_running = false;
  void gameLoop();

  std::unique_ptr<World> m_world;
  std::unique_ptr<ObjectEngine> m_objectEngine;

  std::unique_ptr<PlayerController> m_playerController;

  int m_renderDistance = 4;
  std::vector<ChunkPosition> m_relativeChunkOffsets;
  void calculateRelativeChunkOffsets();
  std::vector<ChunkPosition> m_chunksToRender;
  QMutex m_chunksToRenderMutex;
  void updateChunksToRender();
  std::atomic<bool> m_chunksToRenderDirty = false;
};

#endif // ENGINE_H