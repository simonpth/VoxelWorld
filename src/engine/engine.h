#ifndef ENGINE_H
#define ENGINE_H

#include "playercontroller/renderplayercontroller.h"
#include "data/world.h"
#include "data/chunkmanager.h"

#include <atomic>
#include <memory>

#include <taskflow/taskflow.hpp>

class Engine
{

public:
  Engine();
  ~Engine();

  std::shared_ptr<World> world() { return m_world; }

  void run();
  void stop();

  // DO NOT CALL THIS FROM ANY THREAD OTHER THAN THE RENDERING THREAD OR GAMELOOP THREAD
  PlayerControllerInterface *playerController() { return m_playerController.get(); }

  // DO NOT CALL THIS FROM ANY THREAD OTHER THAN THE RENDERING THREAD OR GAMELOOP THREAD
  ChunkManager *chunkManager() { return m_chunkManager.get(); }

private:
  std::atomic<bool> m_running = false;
  void gameLoop();

  void tick();

  std::shared_ptr<World> m_world;

  std::unique_ptr<PlayerControllerInterface> m_playerController;
  std::unique_ptr<ChunkManager> m_chunkManager;

  tf::Executor m_executor;
};

#endif // ENGINE_H