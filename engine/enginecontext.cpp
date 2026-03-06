#include "enginecontext.h"

EngineContext::EngineContext(QObject *parent) : QObject(parent) {
  m_engine = new Engine();
  m_engineThread = new QThread(this);

  connect(m_engineThread, &QThread::started, m_engine, &Engine::run);

  m_engine->moveToThread(m_engineThread);
  m_engineThread->start();
}

EngineContext::~EngineContext() {
  m_engineThread->quit();
  m_engineThread->wait();
  delete m_engine;
}