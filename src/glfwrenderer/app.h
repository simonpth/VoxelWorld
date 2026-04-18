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

  const Renderer& renderer() const { return m_renderer; }
private:
  void processInput();
  GLFWcursor* createInvisibleCursor();

  GLFWwindow *m_window;
  GLFWcursor *m_invisibleCursor = nullptr;

  Renderer m_renderer;

  bool m_captureFocus = false;
};

#endif // APP_H