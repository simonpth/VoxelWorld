#include "engine.h"

#include <chrono>
#include <memory>

Engine::Engine() {
  m_world = std::make_shared<World>();
  m_objectEngine = std::make_unique<ObjectEngine>();

  m_playerController = std::make_unique<PlayerController>();
}

Engine::~Engine() {
  stop();
}

void Engine::gameLoop() {
  using clock = std::chrono::steady_clock;
  using namespace std::chrono;

  constexpr auto tick = 20ms;

  auto previous = clock::now();
  auto lag = 0ms;

  while (m_running) {
    auto current = clock::now();
    lag += duration_cast<milliseconds>(current - previous);
    previous = current;

    // Catch-up updates
    while (lag >= tick) {
      // qDebug() << "Engine running...";

      // Update game logic here (e.g., physics, AI, world updates)

      lag -= tick;
    }

    // Sleep remaining time if we're ahead
    auto sleepTime = tick - lag;
    // if (sleepTime > 0ms)
    //   sleep_for(sleepTime); --- IGNORE ---
  }
}

void Engine::run() {
  m_running.store(true);
  gameLoop();
}

void Engine::stop() {
  m_running.store(false);
}