#include "enginecontext.h"

EngineContext &EngineContext::instance()
{
  static EngineContext context;
  return context;
}

void EngineContext::deleteEngine()
{
  std::unique_lock lock(m_engineMutex);
  if (m_engine)
  {
    m_engine->stop();
    if (m_engineThread.joinable())
      m_engineThread.join();

    m_engine.reset();
  }
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
  std::shared_lock lock(m_engineMutex);
  return m_engine;
}