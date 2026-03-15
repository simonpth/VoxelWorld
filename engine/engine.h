#ifndef ENGINE_H
#define ENGINE_H

#include "objectengine.h"
#include "playercontroller.h"
#include "world.h"

#include <QObject>
#include <QThread>
#include <QtCore/qmutex.h>
#include <atomic>
#include <memory>
#include <qqmlintegration.h>

class Engine : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(PlayerController *playerController READ playerController CONSTANT)

public:
  Engine(QObject *parent = nullptr);
  ~Engine();

  std::shared_ptr<World> world() { return m_world; }
  ObjectEngine *objectEngine() { return m_objectEngine.get(); }
  PlayerController *playerController() { return m_playerController.get(); }


public slots:
  void run();
  void stop() { m_running = false; }

private:
  QThread *m_gameLoopThread;
  std::atomic<bool> m_running = false;
  void gameLoop();

  std::shared_ptr<World> m_world;
  std::unique_ptr<ObjectEngine> m_objectEngine;

  std::unique_ptr<PlayerController> m_playerController;
};

#endif // ENGINE_H