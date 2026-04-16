#ifndef MAINMENU_H
#define MAINMENU_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

class MainMenu {
public:
  MainMenu(int width, int height, const char *title);
  ~MainMenu();

  bool init();
  void run();
  void shutdown();

  bool shouldStartGame() const { return m_shouldStartGame; }

private:
  void initImGui();
  void shutdownImGui();
  void renderUI();

private:
  GLFWwindow *m_window;
  int m_width;
  int m_height;
  const char *m_title;

  int m_planetSizeInChunks = 512; // Default value, can be changed in the UI
  int m_worldSeed = 1337; // Default value, can be changed in the UI

  bool m_shouldStartGame = false;
};

#endif // MAINMENU_H