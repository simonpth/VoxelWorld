#ifndef DEBUGUI_H
#define DEBUGUI_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

class DebugUI {
public:
  void initialize(GLFWwindow *window);
  void newFrame();
  void render();
  void cleanup();

  bool wantsMouse() const { return ImGui::GetIO().WantCaptureMouse; }

private:
  void drawWidgets();
  int m_renderDistance; // local copy to drive the slider
  bool m_vsync; // local copy to drive the checkbox
  float m_playerSpeed; // local copy to drive the slider
  int m_warpMode; // local copy to drive the dropdown
  bool m_useTextures; // local copy to drive the checkbox
  float m_textureFadeDistance; // local copy to drive the slider
  float m_textureFadeStrength; // local copy to drive the slider

  int m_blockToPlace; // local copy to drive the block selection slider
};

#endif // DEBUGUI_H