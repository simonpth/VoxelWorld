#include "debugui.h"
#include "engine/enginecontext.h"
#include "engine/settings.h"

void DebugUI::initialize(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");

  Settings &settings = Settings::instance();
  m_renderDistance = settings.renderDistance();
  m_vsync = settings.vsync();
  m_playerSpeed = settings.playerSpeed();
  m_warpMode = settings.warpMode();
  m_useTextures = settings.useTextures();
  m_textureFadeDistance = settings.textureFadeDistance();
  m_textureFadeStrength = settings.textureFadeStrength();
  m_blockToPlace = settings.blockToPlace() - 1;
  if (m_vsync) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }

  float xscale, yscale;
  glfwGetWindowContentScale(window, &xscale, &yscale);

  ImGui::GetIO().FontGlobalScale = xscale;
  ImGui::GetStyle().ScaleAllSizes(xscale);
}

void DebugUI::newFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // ImGui::ShowDemoWindow(); // Remove this line to disable the demo window
  drawWidgets();

  // Crosshair
  ImDrawList *draw_list = ImGui::GetForegroundDrawList();

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();

  float size = 10.0f;
  float thickness = 2.0f;
  ImU32 color = IM_COL32(255, 255, 255, 255);

  // Horizontal line
  draw_list->AddLine(
      ImVec2(center.x - size, center.y),
      ImVec2(center.x + size, center.y),
      color,
      thickness);

  // Vertical line
  draw_list->AddLine(
      ImVec2(center.x, center.y - size),
      ImVec2(center.x, center.y + size),
      color,
      thickness);
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

  glm::vec3 worldPos = EngineContext::instance().engine()->playerController()->worldPosition();
  ImGui::Text("World Pos: (%.1f, %.1f, %.1f)", worldPos.x, worldPos.y, worldPos.z);

  if (ImGui::SliderInt("Render Distance", &m_renderDistance, 1, 48)) {
    Settings::instance().setRenderDistance(m_renderDistance);
  }

  if (ImGui::Combo("Block to Place", &m_blockToPlace, "Grass\0Dirt\0Stone\0Snow\0")) {
    Settings::instance().setBlockToPlace(m_blockToPlace + 1);
  }

  if (ImGui::Checkbox("VSync", &m_vsync)) {
    Settings::instance().setVsync(m_vsync);
    if (m_vsync) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }

  if (ImGui::SliderFloat("Player Speed", &m_playerSpeed, 1.0f, 300.0f)) {
    Settings::instance().setPlayerSpeed(m_playerSpeed);
  }

  if (ImGui::Combo("Warp Mode", &m_warpMode, "Flat - No warp\0Warp plane to sphere\0Only move y based on distance\0")) {
    Settings::instance().setWarpMode(m_warpMode);
  }

  if (ImGui::CollapsingHeader("Texture Settings")) {
    if (ImGui::Checkbox("Use Textures", &m_useTextures)) {
      Settings::instance().setUseTextures(m_useTextures);
    }

    if (ImGui::SliderFloat("Texture Fade Distance", &m_textureFadeDistance, 128.0f, 512.0f)) {
      Settings::instance().setTextureFadeDistance(m_textureFadeDistance);
    }

    if (ImGui::SliderFloat("Texture Fade Strength", &m_textureFadeStrength, 0.5f, 5.0f)) {
      Settings::instance().setTextureFadeStrength(m_textureFadeStrength);
    }
  }

  ImGui::End();
}