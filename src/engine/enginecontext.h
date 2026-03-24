#ifndef ENGINECONTEXT_H
#define ENGINECONTEXT_H

#include "engine.h"

#include <memory>
#include <shared_mutex>
#include <thread>

class EngineContext {
public:
  static EngineContext &instance();

  std::thread createEngine();
  void deleteEngine();

  std::shared_ptr<Engine> engine();

private:
  EngineContext() = default;

  std::shared_mutex m_engineMutex;
  std::shared_ptr<Engine> m_engine;

  EngineContext(const EngineContext &) = delete;
  EngineContext &operator=(const EngineContext &) = delete;
};

#endif // ENGINECONTEXT_H