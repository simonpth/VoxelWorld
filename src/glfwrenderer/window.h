#ifndef WINDOW_H
#define WINDOW_H

#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
};

#endif // WINDOW_H