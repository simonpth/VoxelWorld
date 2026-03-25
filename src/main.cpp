#include "glfwrenderer/app.h"
#include "engine/enginecontext.h"

int main()
{
  auto engineThread = EngineContext::instance().createEngine();

  if (!engineThread.joinable())
    return -1;

  App app;
  if (!app.initialize())
    return -1;

  app.mainLoop();
  app.cleanup();

  EngineContext::instance().deleteEngine();
  engineThread.join();

  return 0;
}