#include "vulkanwindow.h"

VulkanWindow::VulkanWindow() : QWindow() {
  setSurfaceType(QSurface::VulkanSurface);
}

