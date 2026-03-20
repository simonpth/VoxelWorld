#include "glfwrenderer/window.h"

int main()
{
  Window window;
  if (!window.initialize())
    return -1;

  window.mainLoop();
  window.cleanup();
  return 0;
}