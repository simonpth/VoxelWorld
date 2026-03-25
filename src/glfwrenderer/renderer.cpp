#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::initialize()
{
  m_firstRender = true;
  m_chunkManager = std::make_unique<ChunkManager>();
  m_chunkManager->setRenderDistance(12);
  m_playerController = std::make_unique<RenderPlayerController>();
  m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");
}

void Renderer::render()
{
  if (m_firstRender)
  {
    m_lastPlayerChunkPos = m_playerController->currentChunk();
    m_chunkManager->updateLoadedMeshes(m_lastPlayerChunkPos);
  }

  auto delta = timeSinceLastFrame();
  updateFps(delta);

  m_playerController->setInputState(m_inputState);
  m_playerController->update();

  // Update loaded chunks if the player has moved to a different chunk
  PlayerChunkPos currentChunkPos = m_playerController->currentChunk();
  if (currentChunkPos != m_lastPlayerChunkPos)
  {
    m_lastPlayerChunkPos = currentChunkPos;
    m_chunkManager->updateLoadedMeshes(currentChunkPos);
  }

  // Calculate the MVP matrix
  glm::vec3 playerPos = m_playerController->position();

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
  glm::mat4 view = glm::lookAt(
      playerPos,
      playerPos + m_playerController->front(),
      m_playerController->up());

  glm::mat4 vp = projection * view;

  // Render the chunks
  glClearColor(0.0f, 0.75f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render solids
  m_shader->use();

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  m_shader->setMat4("vp", vp);

  for (const auto &[chunkPos, chunkMesh] : m_chunkManager->chunkMeshes())
  {
    float relativeX = (chunkPos.x - currentChunkPos.x) * Chunk::SIZE;
    float relativeY = (chunkPos.y - currentChunkPos.y) * Chunk::SIZE;
    float relativeZ = (chunkPos.z - currentChunkPos.z) * Chunk::SIZE;
    m_shader->setVec3("relativeChunkPos", relativeX, relativeY, relativeZ);
    chunkMesh->render();
  }

  // Change at the end so all function calls during the first render see m_firstRender as true
  if (m_firstRender)
    m_firstRender = false;
}

void Renderer::processInput(GLFWwindow *window)
{
  m_inputState.moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  m_inputState.moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  m_inputState.moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  m_inputState.moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
  m_inputState.moveUp = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  m_inputState.moveDown = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  
  if (m_firstMouse)
  {
    m_lastX = xpos;
    m_lastY = ypos;
    m_firstMouse = false;
  }

  float xoffset = xpos - m_lastX;
  float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top
  m_lastX = xpos;
  m_lastY = ypos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  m_playerController->addRotation(glm::vec3(yoffset, xoffset, 0.0f));
}

std::chrono::nanoseconds Renderer::timeSinceLastFrame()
{
  auto now = std::chrono::steady_clock::now();
  if (m_firstRender)
  {
    m_firstRender = false;
    m_lastFrame = now;
    return std::chrono::nanoseconds(0);
  }
  auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastFrame);
  m_lastFrame = now;
  return delta;
}

void Renderer::updateFps(std::chrono::nanoseconds delta)
{
  m_timeSinceLastFpsUpdate += delta;
  m_framesSinceLastFpsUpdate++;
  if (m_timeSinceLastFpsUpdate > std::chrono::nanoseconds(1'000'000'000))
  {
    m_fps = static_cast<int>(m_framesSinceLastFpsUpdate * 1e9 / m_timeSinceLastFpsUpdate.count());
    m_framesSinceLastFpsUpdate = 0;
    m_timeSinceLastFpsUpdate = std::chrono::nanoseconds(0);

    std::println("FPS: {}", m_fps);
  }
}