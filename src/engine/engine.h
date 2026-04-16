#ifndef ENGINE_H
#define ENGINE_H

#include "playercontroller/renderplayercontroller.h"
#include "data/world.h"
#include "data/chunkmanager.h"
#include "data/blockregistry.h"

#include <atomic>
#include <memory>

#include <taskflow/taskflow.hpp>

struct BlockAction {
  glm::ivec3 blockPos;
  bool place; // true for place, false for break
};

class Engine
{
public:
  Engine();
  ~Engine();

  std::shared_ptr<World> world() { return m_world; }

  void run();
  void stop();

  PlayerControllerInterface *playerController() { return m_playerController.get(); }

  // DO NOT CALL THIS FROM ANY THREAD OTHER THAN THE RENDERING THREAD OR GAMELOOP THREAD
  ChunkManager &chunkManager() { return m_chunkManager; }

  const BlockRegistry &blockRegistry() const { return m_blockRegistry; }

  // Thread-safe function to queue a block action (place or break)
  void queueBlockAction(const BlockAction &action) {
    std::lock_guard<std::mutex> lock(m_blockActionQueueMutex);
    m_blockActionQueue.push(action);
  }
  
private:
  std::atomic<bool> m_running = false;
  void gameLoop();

  void tick();

  std::shared_ptr<World> m_world;

  std::unique_ptr<PlayerControllerInterface> m_playerController;
  ChunkManager m_chunkManager;
  BlockRegistry m_blockRegistry;

  tf::Executor m_executor;

  std::queue<BlockAction> m_blockActionQueue;
  std::mutex m_blockActionQueueMutex;
};

#endif // ENGINE_H