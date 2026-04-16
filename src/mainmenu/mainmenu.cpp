#include "mainmenu.h"

#include <iostream>

#include "engine/settings.h"

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

MainMenu::MainMenu(int width, int height, const char *title)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title) {}

MainMenu::~MainMenu() {
  shutdown();
}

bool MainMenu::init() {
  // Set seed for randomization
  srand(static_cast<unsigned int>(time(nullptr)));

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
  if (!m_window) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init GLAD\n";
    return false;
  }

  initImGui();
  return true;
}

void MainMenu::initImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void MainMenu::run() {
  while (!glfwWindowShouldClose(m_window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderUI();

    ImGui::Render();

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
  }
}

// Renders the main menu UI using ImGui - mostly AI
void MainMenu::renderUI() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::Begin("Main Menu", nullptr, window_flags);

  ImGui::Text("Voxel World");

  // Dropdown for planet size (powers of 2 only: 32, 64, 128, 256, 512, 1024)
  const char* planetSizeItems[] = {"32", "64", "128", "256", "512", "1024"};
  int currentPlanetSizeIndex = 0;

  // Find current index
  switch (m_planetSizeInChunks) {
    case 32: currentPlanetSizeIndex = 0; break;
    case 64: currentPlanetSizeIndex = 1; break;
    case 128: currentPlanetSizeIndex = 2; break;
    case 256: currentPlanetSizeIndex = 3; break;
    case 512: currentPlanetSizeIndex = 4; break;
    case 1024: currentPlanetSizeIndex = 5; break;
    default: currentPlanetSizeIndex = 0; break;
  }

  if (ImGui::BeginCombo("Planet Size (chunks)", planetSizeItems[currentPlanetSizeIndex])) {
    for (int i = 0; i < IM_ARRAYSIZE(planetSizeItems); i++) {
      bool isSelected = (currentPlanetSizeIndex == i);
      if (ImGui::Selectable(planetSizeItems[i], isSelected)) {
        currentPlanetSizeIndex = i;
        m_planetSizeInChunks = atoi(planetSizeItems[i]);
        Settings::instance().setPlanetSizeInChunks(m_planetSizeInChunks);
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  if (ImGui::Button("Randomize World Seed")) {
    m_worldSeed = rand() % 1000000;
    Settings::instance().setWorldSeed(m_worldSeed);
  }

  // Text input for world seed (positive integers only)
  char seedBuffer[32];
  snprintf(seedBuffer, sizeof(seedBuffer), "%d", m_worldSeed);
  if (ImGui::InputText("World Seed", seedBuffer, sizeof(seedBuffer), ImGuiInputTextFlags_CharsDecimal)) {
    int newSeed = atoi(seedBuffer);
    if (newSeed >= 0) {  // Only accept positive integers
      m_worldSeed = newSeed;
      Settings::instance().setWorldSeed(m_worldSeed);
    }
  }

  if (ImGui::Button("Start Game")) {
    m_shouldStartGame = true;

    glfwSetWindowShouldClose(m_window, true);
  }

  ImGui::End();
}

void MainMenu::shutdownImGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void MainMenu::shutdown() {
  shutdownImGui();

  if (m_window) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
  }

  glfwTerminate();
}