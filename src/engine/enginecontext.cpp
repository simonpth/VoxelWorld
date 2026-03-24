#include "enginecontext.h"

EngineContext &EngineContext::instance() {
  static EngineContext context;
  return context;
}

void EngineContext::deleteEngine() {
  std::unique_lock lock(m_engineMutex);
  if(m_engine) {
    m_engine.reset();
  }
}

std::thread EngineContext::createEngine() {
  if (!m_engine) {
    std::unique_lock lock(m_engineMutex);
    m_engine = std::make_shared<Engine>();

    std::thread engineThread([this]() {
      m_engine->run();
    });
    return engineThread;
  }
  return std::thread();
}

std::shared_ptr<Engine> EngineContext::engine() {
  std::shared_lock lock(m_engineMutex);
  return m_engine;
}