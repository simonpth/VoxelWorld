#ifndef ENGINE_H
#define ENGINE_H

#include "objectengine.h"
#include "world.h"

#include <QObject>
#include <memory>
#include <qqmlintegration.h>

class Engine : public QObject {
  Q_OBJECT
  QML_ELEMENT
public:
  Engine(QObject *parent = nullptr);

  World *world() { return m_world.get(); }
  ObjectEngine *objectEngine() { return m_objectEngine; }

private:
  std::unique_ptr<World> m_world;
  ObjectEngine *m_objectEngine;
};

#endif // ENGINE_H