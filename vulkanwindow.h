#ifndef VULKANWINDOW_H
#define VULKANWINDOW_H

#include <QWindow>
#include <vulkan/vulkan_raii.hpp>

class VulkanWindow : public QWindow {
  Q_OBJECT

public:
  explicit VulkanWindow();

private:
  void initVulkan();
  void mainLoop();
  void cleanupVulkan();

  void createInstance();

private:
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
};

#endif // VULKANWINDOW_H