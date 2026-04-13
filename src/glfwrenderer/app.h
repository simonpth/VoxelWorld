#ifndef APP_H
#define APP_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "renderer.h"
#include "debugui.h"

class App {
public:
  App();

  bool initialize();
  void mainLoop();
  void cleanup();

  void captureFocus();
  void releaseFocus();

  bool isFocusCaptured() const { return m_captureFocus; }

  void setWindowWidth(int width) { m_renderer.setWindowWidth(width); }
  void setWindowHeight(int height) { m_renderer.setWindowHeight(height); }

  const DebugUI& getDebugUI() const { return m_debugUI; }
private:
  void processInput();

  GLFWwindow *m_window;

  Renderer m_renderer;

  DebugUI m_debugUI;

  bool m_captureFocus = false;
};

#endif // APP_H