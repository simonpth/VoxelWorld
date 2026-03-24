#ifndef WINDOW_H
#define WINDOW_H

#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>

#include "chunkmesh.h"
#include "renderplayercontroller.h"

#include "engine/chunk.h"
#include <memory>

class Window
{
public:
  Window();

  bool initialize();
  void mainLoop();
  void cleanup();

private:
  void processInput();

  GLFWwindow* m_window;
  
  std::unique_ptr<Shader> m_shader;

  std::chrono::steady_clock::time_point m_lastFpsUpdate;
  int m_frameCountSinceLastFpsUpdate = 0;

  std::chrono::steady_clock::time_point m_lastFrame;

  RenderPlayerControllerInputState m_inputState;
  RenderPlayerController m_playerController;
  std::unordered_map<ChunkPosition, std::unique_ptr<ChunkMesh>> m_chunkMeshes;
};

#endif // WINDOW_H