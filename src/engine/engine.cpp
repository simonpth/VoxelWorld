#include "engine.h"

#include <chrono>
#include <memory>
#include <thread>

#include "settings.h"

Engine::Engine() {
  m_world = std::make_shared<World>();
  m_playerController = std::make_unique<RenderPlayerController>();
  m_chunkManager = std::make_unique<ChunkManager>();
}

Engine::~Engine() {
}

// Main game loop with fixed time step, calls tick() every 20ms
void Engine::gameLoop() {
  using clock = std::chrono::steady_clock;
  using namespace std::chrono;

  constexpr auto tickTime = 20ms;

  auto previous = clock::now();
  auto lag = 0ms;

  while (m_running) {
    auto current = clock::now();
    lag += duration_cast<milliseconds>(current - previous);
    previous = current;

    // Catch-up updates
    while (lag >= tickTime) {
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
void Engine::tick() {
  tf::Taskflow updateTaskflow;

  // Check if the player has moved to a different chunk -> update loaded chunks
  updateTaskflow.emplace([this]() {
    if (m_playerController->chunkChanged()) {
      m_playerController->resetChunkChanged();
      m_chunkManager->updateLoadedMeshes(m_playerController->currentChunk());
    }
  });

  // Check if the render distance has changed -> update loaded chunks
  updateTaskflow.emplace([this]() {
    if (m_chunkManager->renderDistance() != Settings::instance().renderDistance()) {
      m_chunkManager->setRenderDistance(Settings::instance().renderDistance());
      m_chunkManager->updateLoadedMeshes(m_playerController->currentChunk());
    }
  });

  // Example of setting a block in front of the player every tick (for testing)
  updateTaskflow.emplace([this]() {
    auto playerChunkPos = m_playerController->currentChunk();
    glm::vec3 pos = m_playerController->position() + m_playerController->front() * 5.0f; // Position 5 units in front of the player
    glm::ivec3 worldPos;
    worldPos.x = std::round(pos.x) + playerChunkPos.x() * Chunk::SIZE;
    worldPos.y = std::round(pos.y) + playerChunkPos.y() * Chunk::SIZE;
    worldPos.z = std::round(pos.z) + playerChunkPos.z() * Chunk::SIZE;
    m_chunkManager->setBlockAndUpdate(worldPos, Block(4));
  });

  // Wait for all update tasks to finish before the next tick
  m_executor.run(updateTaskflow).wait();
}

void Engine::run() {
  m_running.store(true);
  gameLoop();
}

void Engine::stop() {
  m_running.store(false);
}