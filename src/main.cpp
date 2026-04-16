#include "engine/enginecontext.h"
#include "glfwrenderer/app.h"
#include "mainmenu/mainmenu.h"

int main() {
  bool startGame = false;
  {
    MainMenu mainMenu(800, 600, "Voxel World - Main Menu");
    if (!mainMenu.init())
      return -1;
    mainMenu.run();

    startGame = mainMenu.shouldStartGame();
  }

  if (!startGame) {
    return 0; // Exit if the player didn't choose to start the game
  }

  EngineContext::instance().createEngine();

  App app;
  if (!app.initialize())
    return -1;

  app.mainLoop();
  app.cleanup();

  EngineContext::instance().deleteEngine();
  // ^ Waits until the engine thread finishes before exiting the program

  return 0;
}