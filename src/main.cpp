#include "glfwrenderer/app.h"
#include "engine/enginecontext.h"

int main()
{
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