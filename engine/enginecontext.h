#ifndef ENGINECONTEXT_H
#define ENGINECONTEXT_H

#include "engine.h"

#include <QObject>
#include <QThread>
#include <QtCore/qthread.h>
#include <qqmlintegration.h>

class EngineContext : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(Engine *engine READ engine CONSTANT)

public:
  EngineContext(QObject *parent = nullptr);
  ~EngineContext();

  Engine *engine() { return m_engine; }

private:
  Engine *m_engine;
  QThread *m_engineThread;
};

#endif // ENGINECONTEXT_H