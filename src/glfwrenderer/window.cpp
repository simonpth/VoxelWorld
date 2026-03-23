#include "window.h"

#include <iostream>

Window::Window()
{
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

bool Window::initialize()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_window = glfwCreateWindow(800, 600, "Voxel World", nullptr, nullptr);
  if (m_window == nullptr)
  {
    std::println("Failed to create GLFW window");
    return false;
  }
  glfwMakeContextCurrent(m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::println("Failed to initialize GLAD");
    return false;
  }
  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

  m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");

  return true;
}

void Window::processInput()
{
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(m_window, true);
  }
}

void Window::mainLoop()
{
  while (!glfwWindowShouldClose(m_window))
  {
    processInput();

    // light blue sky background
    glClearColor(0.0f, 0.75f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->use();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void Window::cleanup()
{
  glfwDestroyWindow(m_window);
  m_window = nullptr;
  glfwTerminate();
}