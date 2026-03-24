#ifndef ENGINE_H
#define ENGINE_H

#include "objectengine.h"
#include "playercontroller.h"
#include "world.h"

#include <atomic>
#include <memory>

class Engine {

public:
  Engine();
  ~Engine();

  std::shared_ptr<World> world() { return m_world; }
  ObjectEngine *objectEngine() { return m_objectEngine.get(); }
  PlayerController *playerController() { return m_playerController.get(); }

  void run();
  void stop();

private:
  std::atomic<bool> m_running = false;
  void gameLoop();

  std::shared_ptr<World> m_world;
  std::unique_ptr<ObjectEngine> m_objectEngine;

  std::unique_ptr<PlayerController> m_playerController;
};

#endif // ENGINE_H