#ifndef APP_H
#define APP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"

class App
{
public:
  App();

  bool initialize();
  void mainLoop();
  void cleanup();

  void captureFocus();
  void releaseFocus();

  bool isFocusCaptured() const { return m_captureFocus; }

private:
  void processInput();

  GLFWwindow* m_window;

  Renderer m_renderer;

  bool m_captureFocus = false;
};

#endif // APP_H