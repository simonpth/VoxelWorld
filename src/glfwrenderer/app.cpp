#include "app.h"

#include "engine/constants.h"
#include "engine/enginecontext.h"

#include <iostream>

App::App() {
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->setWindowWidth(width);
    app->setWindowHeight(height);
  }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
    if (app->renderer().uiWantCaptureMouse())
      return;
    if (app) {
      app->captureFocus();
    }
  }
}

bool App::initialize() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_window = glfwCreateWindow(800, 600, "Voxel World", nullptr, nullptr);
  m_renderer.setWindowWidth(800);
  m_renderer.setWindowHeight(600);
  if (m_window == nullptr) {
    std::println(std::cout, "Failed to create GLFW window");
    return false;
  }
  glfwMakeContextCurrent(m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println(std::cout, "Failed to initialize GLAD");
    return false;
  }

  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
  glfwSetMouseButtonCallback(m_window, mouse_button_callback);

  glfwSetWindowUserPointer(m_window, this);
  m_renderer.initialize(m_window);

  return true;
}

void App::mainLoop() {
  while (!glfwWindowShouldClose(m_window)) {
    if (m_captureFocus) {
      processInput();
      m_renderer.processInput(m_window);
    }

    if (m_captureFocus)
      ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    else
      ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

    m_renderer.render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void App::cleanup() {
  m_renderer.cleanup();
  glfwDestroyWindow(m_window);
  m_window = nullptr;
  glfwTerminate();
}

void App::processInput() {
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    releaseFocus();
  }
}

void App::captureFocus() {
  if (!m_captureFocus) {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_captureFocus = true;
  }
}

void App::releaseFocus() {
  if (m_captureFocus) {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_captureFocus = false;
  }
}