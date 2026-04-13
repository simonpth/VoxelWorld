#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/enginecontext.h"

void Renderer::initialize() {
  m_firstRender = true;

  m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");
}

void Renderer::render() {
  auto delta = timeSinceLastFrame();
  updateFps(delta);

  auto playerController = EngineContext::instance().engine()->playerController();
  playerController->update();

  // Update loaded chunks if the player has moved to a different chunk
  PlayerChunkPos currentChunkPos = playerController->currentChunk();

  // Calculate the MVP matrix
  glm::vec3 playerPos = playerController->position();

  float aspectRatio = static_cast<float>(m_windowWidth.load()) / static_cast<float>(m_windowHeight.load());
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
  glm::mat4 view = glm::lookAt(
      playerPos,
      playerPos + playerController->front(),
      playerController->up());

  glm::mat4 vp = projection * view;

  // Render the chunks
  glClearColor(0.0f, 0.75f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render solids
  m_shader->use();

  glDisable(GL_BLEND);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  m_shader->setMat4("vp", vp);

  auto chunkManager = EngineContext::instance().engine()->chunkManager();

  if (chunkManager->loadedChunkVersion() != m_chunkManagerChunkVersion) {
    m_chunkManagerChunkVersion = chunkManager->loadedChunkVersion();

    std::queue<LoadedChunkUpdate> updates;
    {
      LoadedChunkUpdatesReadHandle handle = chunkManager->getLoadedChunkUpdates();
      std::swap(updates, *handle.loadedChunkUpdates);
    }

    while (!updates.empty()) {
      const LoadedChunkUpdate &update = updates.front();
      for (const auto &[pos, vertices] : update.chunksToLoad) {
        auto chunkMesh = std::make_unique<ChunkRenderMesh>();
        chunkMesh->setChunkVertices(vertices);
        chunkMesh->initialize();
        m_chunkMeshes[pos] = std::move(chunkMesh);
      }
      for (const ChunkPosition &pos : update.chunksToUnload) {
        m_chunkMeshes.erase(pos);
      }
      updates.pop();
    }
  }

  for (const auto &[chunkPos, chunkMesh] : m_chunkMeshes) {
    chunkMesh->uploadVerticesIfNeeded();

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

void Renderer::processInput(GLFWwindow *window) {
  auto playerController = EngineContext::instance().engine()->playerController();

  PlayerControllerInput inputState;
  inputState.moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  inputState.moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  inputState.moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  inputState.moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
  inputState.moveUp = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  inputState.moveDown = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

  playerController->setInputState(inputState);

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  if (m_firstMouse) {
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

  playerController->addRotation(glm::vec3(yoffset, xoffset, 0.0f));
}

std::chrono::nanoseconds Renderer::timeSinceLastFrame() {
  auto now = std::chrono::steady_clock::now();
  if (m_firstRender) {
    m_firstRender = false;
    m_lastFrame = now;
    return std::chrono::nanoseconds(0);
  }
  auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastFrame);
  m_lastFrame = now;
  return delta;
}

void Renderer::updateFps(std::chrono::nanoseconds delta) {
  m_timeSinceLastFpsUpdate += delta;
  m_framesSinceLastFpsUpdate++;
  if (m_timeSinceLastFpsUpdate > std::chrono::nanoseconds(1'000'000'000)) {
    m_fps = static_cast<int>(m_framesSinceLastFpsUpdate * 1e9 / m_timeSinceLastFpsUpdate.count());
    m_framesSinceLastFpsUpdate = 0;
    m_timeSinceLastFpsUpdate = std::chrono::nanoseconds(0);

    std::println(std::cout, "FPS: {}", m_fps);
  }
}