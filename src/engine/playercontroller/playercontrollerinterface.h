#ifndef PLAYERCONTROLLERINTERFACE_H
#define PLAYERCONTROLLERINTERFACE_H

// Abstract interface for player controller, so that the game logic can be decoupled from the rendering code

#include <glm/glm.hpp>
#include <atomic>
#include "engine/data/playerchunkpos.h"


struct PlayerControllerInput {
  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool jump = false;
};

// Usage - every frame
// 1. Call setInputState() with the current input state (which keys are pressed)
// 2. Call update() with the time delta since the last frame
// 3. Call position(), rotation(), front(), up(), and currentChunk()
//   to get the player's current position, rotation, and chunk

class PlayerControllerInterface
{
public:
  virtual ~PlayerControllerInterface() = default;

  // should be called every frame with the current input state
  virtual void update() = 0;
  virtual void setInputState(const PlayerControllerInput &inputState) = 0;

  // after every mouse movement
  virtual void addRotation(const glm::vec3 &rotation) = 0;

  // getters can be called from any thread
  virtual glm::vec3 position() const = 0;
  virtual glm::vec3 rotation() const = 0;
  virtual glm::vec3 front() const = 0;
  virtual glm::vec3 up() const = 0;
  virtual glm::vec3 worldPosition() const = 0;
  virtual PlayerChunkPos currentChunk() const = 0;
  
  virtual bool chunkChanged() const {
    return m_chunkChanged.load();
  }
  virtual void resetChunkChanged() {
    m_chunkChanged.store(false);
  }

protected:
  PlayerControllerInput m_inputState;
  std::atomic<bool> m_chunkChanged = false;
};

#endif // PLAYERCONTROLLERINTERFACE_H