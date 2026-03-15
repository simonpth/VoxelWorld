#include "enginecontext.h"
#include <QtCore/qreadwritelock.h>
#include <memory>

EngineContext &EngineContext::instance() {
  static EngineContext context;
  return context;
}

void EngineContext::deleteEngine() {
  QWriteLocker locker(&m_lock);
  if(m_engine) {
    m_engine.reset();
  }
  if(m_engineThread) {
    m_engineThread->quit();
    m_engineThread->wait();
    m_engineThread.reset();
  }
  m_isEngineCreated = false;
}

void EngineContext::createEngine() {
  if (!m_isEngineCreated) {
    QWriteLocker locker(&m_lock);
    m_engine = std::make_shared<Engine>();
    m_engineThread = std::make_unique<QThread>();
    m_engineThread.get()->connect(m_engineThread.get(), &QThread::started,
                                  m_engine.get(), &Engine::run);
    m_engine->moveToThread(m_engineThread.get());
    m_engineThread->start();
    m_isEngineCreated = true;
  }
}

std::shared_ptr<Engine> EngineContext::engine() {
  QReadLocker locker(&m_lock);
  return m_engine;
}