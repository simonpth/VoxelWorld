#ifndef RENDERER_H
#define RENDERER_H

#include "chunkrendermesh.h"
#include "engine/playercontroller/renderplayercontroller.h"
#include "shader.h"
#include "debugui.h"
#include "frustum.h"
#include "blockregistrytbo.h"
#include "textureatlas.h"

#include <atomic>
#include <chrono>

#include <GLFW/glfw3.h>

class Renderer
{
public:
  Renderer() = default;

  void initialize(GLFWwindow *window);
  void render();
  void cleanup();

  void processInput(GLFWwindow* window);

  void setWindowWidth(int width) { m_windowWidth.store(width); }
  void setWindowHeight(int height) { m_windowHeight.store(height); }

  bool uiWantCaptureMouse() const { return m_debugUI.wantsMouse(); }

private:
  // Player controller inputs
  bool m_firstRender = true;
  double m_lastX = 0;
  double m_lastY = 0;
  bool m_firstMouse = true;

  // Rendering resources
  std::atomic<int> m_windowWidth, m_windowHeight;

  std::unique_ptr<Shader> m_shader;

  std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRenderMesh>> m_chunkMeshes;

  int m_chunkManagerChunkVersion = 0; // Version of chunk data currently rendered

  Frustum m_frustum; // For culling

  // Dear ImGui resources
  DebugUI m_debugUI;

  // Fog
  int m_fogRenderDistance;

  // Textures
  BlockRegistryTBO m_blockRegistryTBO;
  TextureAtlas m_textureAtlas;
};

#endif // RENDERER_H