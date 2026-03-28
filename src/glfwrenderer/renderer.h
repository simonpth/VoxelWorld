#ifndef RENDERER_H
#define RENDERER_H

#include "chunkrendermesh.h"
#include "engine/playercontroller/renderplayercontroller.h"
#include "shader.h"

#include <atomic>
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

  void setWindowWidth(int width) { m_windowWidth.store(width); }
  void setWindowHeight(int height) { m_windowHeight.store(height); }

private:
  // FPS tracking
  std::chrono::nanoseconds timeSinceLastFrame();
  void updateFps(std::chrono::nanoseconds delta);

  std::chrono::steady_clock::time_point m_lastFrame;
  int m_fps = 0;
  std::chrono::nanoseconds m_timeSinceLastFpsUpdate = std::chrono::nanoseconds(0);
  int m_framesSinceLastFpsUpdate = 0;

  // Player controller inputs
  bool m_firstRender = true;
  double m_lastX = 0;
  double m_lastY = 0;
  bool m_firstMouse = true;

  // Rendering resources
  std::atomic<int> m_windowWidth, m_windowHeight;

  std::unique_ptr<Shader> m_shader;

  std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRenderMesh>> m_chunkMeshes;
};

#endif // RENDERER_H