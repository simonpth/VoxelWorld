#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/enginecontext.h"

static glm::vec3 skyColor = glm::vec3(0.0f, 0.75f, 1.0f);

void Renderer::initialize(GLFWwindow *window) {
  m_firstRender = true;

  m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");

  m_shader->use();
  m_shader->setVec3("fogColor", skyColor);
  m_fogRenderDistance = Settings::instance().renderDistance() * Chunk::SIZE;
  m_shader->setFloat("fogStart", m_fogRenderDistance - Chunk::SIZE);
  m_shader->setFloat("fogEnd", m_fogRenderDistance);

  m_debugUI.initialize(window);

  m_blockRegistryTBO.initialize(EngineContext::instance().engine()->blockRegistry());

  m_shader->setInt("blockTextureTBO", 0); // Texture unit 0
}

void Renderer::render() {
  // Start a new ImGui frame and draw debug widgets
  m_debugUI.newFrame();

  // Voxel World rendering
  auto playerController = EngineContext::instance().engine()->playerController();
  playerController->update();

  // Update loaded chunks if the player has moved to a different chunk
  PlayerChunkPos currentChunkPos = playerController->currentChunk();

  // Calculate the MVP matrix
  glm::vec3 playerPos = playerController->position();

  float aspectRatio = static_cast<float>(m_windowWidth.load()) / static_cast<float>(m_windowHeight.load());
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 2000.0f);
  glm::mat4 view = glm::lookAt(
      playerPos,
      playerPos + playerController->front(),
      playerController->up());

  glm::mat4 vp = projection * view;

  m_frustum.extractFrustum(vp);

  // Render the chunks
  glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render solids
  m_shader->use();

  // Bind the block registry texture buffer object to texture unit 0
  m_blockRegistryTBO.bind(0);

  // Update fog parameters in case render distance changed
  int renderDistance = Settings::instance().renderDistance() * Chunk::SIZE;
  if (m_fogRenderDistance != renderDistance) {
    m_fogRenderDistance = renderDistance;
    int fogDelta = 3 * Chunk::SIZE;
    if (m_fogRenderDistance > 18) {
      fogDelta = 6 * Chunk::SIZE;
    }
    m_shader->setFloat("fogStart", m_fogRenderDistance - fogDelta);
    m_shader->setFloat("fogEnd", m_fogRenderDistance);
  }

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

    if (!m_frustum.aabbInFrustum(
            glm::vec3(relativeX, relativeY, relativeZ),
            glm::vec3(relativeX + Chunk::SIZE, relativeY + Chunk::SIZE, relativeZ + Chunk::SIZE))) {
      continue; // Skip chunks outside the view frustum
    }

    m_shader->setVec3("relativeChunkPos", relativeX, relativeY, relativeZ);
    chunkMesh->render();
  }

  // Render debug UI on top of everything else
  m_debugUI.render();

  // Change at the end so all function calls during the first render see m_firstRender as true
  if (m_firstRender)
    m_firstRender = false;
}

void Renderer::cleanup() {
  m_debugUI.cleanup();
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