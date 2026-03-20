#include "engine.h"

#include <QtCore/qmutex.h>
#include <chrono>
#include <memory>

Engine::Engine(QObject *parent) : QObject(parent) {
  m_world = std::make_shared<World>();
  m_objectEngine = std::make_unique<ObjectEngine>(this);

  m_playerController = std::make_unique<PlayerController>(this);

  m_gameLoopThread = QThread::create([this]() { gameLoop(); });
}

Engine::~Engine() {
  stop();
  if (m_gameLoopThread->isRunning()) {
    m_gameLoopThread->quit();
    m_gameLoopThread->wait();
  }
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
    if (sleepTime > 0ms)
      QThread::msleep(duration_cast<milliseconds>(sleepTime).count());
  }
  qDebug() << "Engine stopped.";
}

void Engine::run() {
  m_running = true;
  if (!m_gameLoopThread->isRunning()) {
    m_gameLoopThread->start();
  }
}