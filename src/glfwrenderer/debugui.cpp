#include "debugui.h"
#include "engine/settings.h"

void DebugUI::initialize(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");

  if (m_vsync) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }

  Settings &settings = Settings::instance();
  m_renderDistance = settings.renderDistance();
  m_vsync = settings.vsync();
  m_playerSpeed = settings.playerSpeed();
}

void DebugUI::newFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // ImGui::ShowDemoWindow(); // Remove this line to disable the demo window
  drawWidgets();
}

void DebugUI::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUI::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void DebugUI::drawWidgets() {
  ImGui::Begin("Debug");
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

  if (ImGui::SliderInt("Render Distance", &m_renderDistance, 1, 32)) {
    Settings::instance().setRenderDistance(m_renderDistance);
  }

  if (ImGui::Checkbox("VSync", &m_vsync)) {
    Settings::instance().setVsync(m_vsync);
    if (m_vsync) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }

  if (ImGui::SliderFloat("Player Speed", &m_playerSpeed, 1.0f, 100.0f)) {
    Settings::instance().setPlayerSpeed(m_playerSpeed);
  }

  ImGui::End();
}