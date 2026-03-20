#ifndef ENGINECONTEXT_H
#define ENGINECONTEXT_H

#include "engine.h"

#include <QObject>
#include <QThread>
#include <QtCore/qreadwritelock.h>
#include <memory>
#include <qqmlintegration.h>

class EngineContext {
public:
  static EngineContext &instance();

  void createEngine();
  void deleteEngine();

  std::shared_ptr<Engine> engine();

private:
  EngineContext() = default;

  QReadWriteLock m_lock;
  std::shared_ptr<Engine> m_engine;
  std::unique_ptr<QThread> m_engineThread;

  bool m_isEngineCreated = false;

  EngineContext(const EngineContext &) = delete;
  EngineContext &operator=(const EngineContext &) = delete;
};

#endif // ENGINECONTEXT_H