#include "engine.h"

Engine::Engine(QObject *parent) : QObject(parent) {
  m_world = std::make_unique<World>();
  m_objectEngine = new ObjectEngine(this);
}