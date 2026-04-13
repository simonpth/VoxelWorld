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
};

#endif // DEBUGUI_H