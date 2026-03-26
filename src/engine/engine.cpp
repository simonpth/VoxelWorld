#include "engine.h"

#include <chrono>
#include <memory>
#include <thread>
#include <ostream>

Engine::Engine()
{
  m_world = std::make_shared<World>();
  m_playerController = std::make_unique<RenderPlayerController>();
  m_chunkManager = std::make_unique<ChunkManager>();
}

Engine::~Engine()
{
}

// Main game loop with fixed time step, calls tick() every 20ms
void Engine::gameLoop()
{
  // Initial chunk loading around the player
  m_chunkManager->setRenderDistance(8);
  m_chunkManager->updateLoadedMeshes(m_playerController->currentChunk());

  using clock = std::chrono::steady_clock;
  using namespace std::chrono;

  constexpr auto tickTime = 20ms;

  auto previous = clock::now();
  auto lag = 0ms;

  while (m_running)
  {
    auto current = clock::now();
    lag += duration_cast<milliseconds>(current - previous);
    previous = current;

    // Catch-up updates
    while (lag >= tickTime)
    {
      // Update game logic here (e.g., physics, AI, world updates)
      tick();

      lag -= tickTime;
    }

    // Sleep remaining time if we're ahead
    auto sleepTime = tickTime - lag;
    if (sleepTime > 0ms)
      std::this_thread::sleep_for(sleepTime);
  }
}

// Runs every tick (20ms) to update game logic
void Engine::tick()
{
  std::vector<std::thread> updateThreads;

  // Check if the player has moved to a different chunk and update loaded meshes if necessary
  if (m_playerController->chunkChanged())
  {
    m_playerController->resetChunkChanged();

    std::thread updateLoadedMeshesThread([this]()
                                         { m_chunkManager->updateLoadedMeshes(m_playerController->currentChunk()); });
    updateThreads.push_back(std::move(updateLoadedMeshesThread));
  }

  // Wait for all update threads to finish before the next tick
  for (auto &thread : updateThreads)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }
}

void Engine::run()
{
  m_running.store(true);
  gameLoop();
}

void Engine::stop()
{
  m_running.store(false);
}