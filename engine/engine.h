#ifndef ENGINE_H
#define ENGINE_H

#include "objectengine.h"
#include "playercontroller.h"
#include "world.h"

#include <QObject>
#include <memory>
#include <qqmlintegration.h>

class Engine : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(PlayerController *playerController READ playerController CONSTANT)

public:
  Engine(QObject *parent = nullptr);

  World *world() { return m_world.get(); }
  ObjectEngine *objectEngine() { return m_objectEngine; }
  PlayerController *playerController() { return m_playerController.get(); }

private:
  std::unique_ptr<World> m_world;
  ObjectEngine *m_objectEngine;

  std::unique_ptr<PlayerController> m_playerController;
};

#endif // ENGINE_H