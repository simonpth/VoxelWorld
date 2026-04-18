#include "enginecontext.h"

EngineContext &EngineContext::instance()
{
  static EngineContext context;
  return context;
}

void EngineContext::deleteEngine()
{
  m_deletingEngine.store(true);
  std::unique_lock lock(m_engineMutex);
  if (m_engine)
  {
    m_engine->stop();
    if (m_engineThread.joinable())
      m_engineThread.join();

    m_engine.reset();
  }
  m_deletingEngine.store(false);
}

void EngineContext::createEngine()
{
  std::unique_lock lock(m_engineMutex);
  if (!m_engine)
  {
    m_engine = std::make_shared<Engine>();

    std::shared_ptr<Engine> engineCopy = m_engine; // Capture a copy for the thread
    m_engineThread = std::thread([engineCopy]()
                                 { engineCopy->run(); });
  }
}

std::shared_ptr<Engine> EngineContext::engine()
{
  if(m_deletingEngine.load()) {
     // Return nullptr if the engine is being deleted to avoid locking issues
    return nullptr;
  }
  std::shared_lock lock(m_engineMutex);
  return m_engine;
}