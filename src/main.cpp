#include "glfwrenderer/window.h"
#include "engine/enginecontext.h"

int main()
{
  auto engineThread = EngineContext::instance().createEngine();

  if (!engineThread.joinable())
    return -1;

  Window window;
  if (!window.initialize())
    return -1;

  window.mainLoop();
  window.cleanup();

  EngineContext::instance().deleteEngine();
  engineThread.join();

  return 0;
}