#ifndef RENDERER_H
#define RENDERER_H

#include "chunkmanager.h"
#include "engine/playercontrollerinterface.h"
#include "renderplayercontroller.h"
#include "shader.h"

#include <chrono>

#include <GLFW/glfw3.h>

class Renderer
{
public:
  Renderer() = default;

  int fps() const { return m_fps; }

  void initialize();
  void render();

  void processInput(GLFWwindow* window);

private:
  std::chrono::nanoseconds timeSinceLastFrame();
  void updateFps(std::chrono::nanoseconds delta);

  std::chrono::steady_clock::time_point m_lastFrame;
  int m_fps = 0;
  std::chrono::nanoseconds m_timeSinceLastFpsUpdate = std::chrono::nanoseconds(0);
  int m_framesSinceLastFpsUpdate = 0;

  std::unique_ptr<ChunkManager> m_chunkManager;

  std::unique_ptr<PlayerControllerInterface> m_playerController;
  PlayerControllerInput m_inputState;
  PlayerChunkPos m_lastPlayerChunkPos;
  bool m_firstRender = true;

  double m_lastX = 0;
  double m_lastY = 0;
  bool m_firstMouse = true;

  std::unique_ptr<Shader> m_shader;
};

#endif // RENDERER_H