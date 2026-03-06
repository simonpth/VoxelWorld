#include "engine.h"

#include <chrono>

Engine::Engine(QObject *parent) : QObject(parent) {
  m_world = std::make_unique<World>();
  m_objectEngine = std::make_unique<ObjectEngine>(this);

  m_playerController = std::make_unique<PlayerController>(this);
  connect(m_playerController.get(), &PlayerController::chunkChanged, this,
          &Engine::onPlayerChunkChanged);

  m_gameLoopThread = QThread::create([this]() { gameLoop(); });

  calculateRelativeChunkOffsets();
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

void Engine::calculateRelativeChunkOffsets() {
  // Generate relative chunk offsets within the render distance
  // This is not the most efficient way to do this, but it works for now
  m_relativeChunkOffsets.clear();
  m_relativeChunkOffsets.reserve((2 * m_renderDistance + 1) *
                                 (2 * m_renderDistance + 1) *
                                 (2 * m_renderDistance + 1));
  for (int x = -m_renderDistance; x <= m_renderDistance; ++x) {
    for (int y = -m_renderDistance; y <= m_renderDistance; ++y) {
      for (int z = -m_renderDistance; z <= m_renderDistance; ++z) {
        if (x * x + y * y + z * z <= m_renderDistance * m_renderDistance) {
          m_relativeChunkOffsets.emplace_back(x, y, z);
        }
      }
    }
  }
  m_relativeChunkOffsets.shrink_to_fit();
}

void Engine::updateChunksToRender() {
  QMutexLocker locker(&m_chunksToRenderMutex);
  m_chunksToRender.clear();
  m_chunksToRender.reserve(m_relativeChunkOffsets.size());
  PlayerChunkPos playerChunk = m_playerController->currentChunk();
  for (const auto &offset : m_relativeChunkOffsets) {
    if (playerChunk.y + offset.y < 0 ||
        playerChunk.y + offset.y >= World::CHUNKHEIGHT)
      continue; // skip chunks below y=0 or above the world height
    m_chunksToRender.emplace_back(playerChunk.x + offset.x,
                                  playerChunk.y + offset.y,
                                  playerChunk.z + offset.z);
  }
  m_chunksToRenderDirty = true;
  qDebug() << "Updated chunks to render. Count:" << m_chunksToRender.size();
}