#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/enginecontext.h"

static glm::vec3 skyColor = glm::vec3(0.145f, 0.655f, 0.855f);

void Renderer::initialize(GLFWwindow *window) {
  const Settings &settings = Settings::instance();
  m_firstRender = true;
  m_planetSizeInChunks = settings.planetSizeInChunks();

  m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");

  m_shader->use();
  m_shader->setVec3("fogColor", skyColor);
  m_fogRenderDistance = settings.renderDistance() * Chunk::SIZE;
  m_shader->setFloat("fogStart", m_fogRenderDistance - Chunk::SIZE);
  m_shader->setFloat("fogEnd", m_fogRenderDistance);

  m_debugUI.initialize(window);

  m_blockRegistryTBO.initialize(EngineContext::instance().engine()->blockRegistry());
  m_shader->setInt("blockTextureTBO", 0); // Texture unit 0

  m_textureAtlas.initialize("shaders/textures/atlas.png");
  m_shader->setInt("blockTextureAtlas", 1); // Texture unit 1

  m_shader->setFloat("planetRadius", settings.planetSizeInChunks() * Chunk::SIZE / 6.2831853072f); // Example planet radius, adjust as needed
}

void Renderer::render() {
  // Start a new ImGui frame and draw debug widgets
  m_debugUI.newFrame();

  // Voxel World rendering
  auto playerController = EngineContext::instance().engine()->playerController();
  playerController->update();

  PlayerChunkPos currentChunkPos = playerController->currentChunk();

  // Calculate the MVP matrix
  glm::vec3 playerPos = playerController->position();

  float aspectRatio = static_cast<float>(m_windowWidth.load()) / static_cast<float>(m_windowHeight.load());
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 7500.0f);
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

  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  const Settings &settings = Settings::instance();

  m_shader->setMat4("vp", vp);
  m_shader->setFloat("playerWorldY", playerPos.y + currentChunkPos.y() * Chunk::SIZE); // player world Y position
  m_shader->setVec3("playerPos", playerPos);                                           // player position relative to current chunk origin
  int warpMode = settings.warpMode();
  m_shader->setInt("warpMode", warpMode);
  m_shader->setBool("useTextures", settings.useTextures());
  m_shader->setFloat("textureFadeDistance", settings.textureFadeDistance());
  m_shader->setFloat("textureFadeStrength", settings.textureFadeStrength());

  // Bind the block registry texture buffer object to texture unit 0
  m_blockRegistryTBO.bind(0);
  m_textureAtlas.bind(1);

  // Update fog parameters in case render distance changed
  int renderDistance = settings.renderDistance() * Chunk::SIZE;
  if (m_fogRenderDistance != renderDistance) {
    m_fogRenderDistance = renderDistance;
    int fogDelta = 3 * Chunk::SIZE;
    if (m_fogRenderDistance > 18) {
      fogDelta = 6 * Chunk::SIZE;
    }
    m_shader->setFloat("fogStart", m_fogRenderDistance - fogDelta);
    m_shader->setFloat("fogEnd", m_fogRenderDistance);
  }

  ChunkManager &chunkManager = EngineContext::instance().engine()->chunkManager();

  if (chunkManager.loadedChunkVersion() != m_chunkManagerChunkVersion) {
    m_chunkManagerChunkVersion = chunkManager.loadedChunkVersion();

    std::queue<LoadedChunkUpdate> updates;
    {
      LoadedChunkUpdatesReadHandle handle = chunkManager.getLoadedChunkUpdates();
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

    float relativeY = (chunkPos.y() - currentChunkPos.y()) * Chunk::SIZE;

    float relativeXP, relativeXN, relativeZP, relativeZN;

    if (chunkPos.x() <= currentChunkPos.x()) {
      relativeXN = (chunkPos.x() - currentChunkPos.x()) * Chunk::SIZE;
      relativeXP = relativeXN + m_planetSizeInChunks * Chunk::SIZE;
    } else {
      relativeXP = (chunkPos.x() - currentChunkPos.x()) * Chunk::SIZE;
      relativeXN = relativeXP - m_planetSizeInChunks * Chunk::SIZE;
    }

    if (chunkPos.z() <= currentChunkPos.z()) {
      relativeZN = (chunkPos.z() - currentChunkPos.z()) * Chunk::SIZE;
      relativeZP = relativeZN + m_planetSizeInChunks * Chunk::SIZE;
    } else {
      relativeZP = (chunkPos.z() - currentChunkPos.z()) * Chunk::SIZE;
      relativeZN = relativeZP - m_planetSizeInChunks * Chunk::SIZE;
    }

    float relativeX = std::abs(relativeXP) < std::abs(relativeXN) ? relativeXP : relativeXN;
    float relativeZ = std::abs(relativeZP) < std::abs(relativeZN) ? relativeZP : relativeZN;

    glm::vec3 relChunkPos = glm::vec3(relativeX, relativeY, relativeZ);

    if (m_planetSizeInChunks >= 256 && !m_frustum.aabbInFrustum(
                            relChunkPos,
                            relChunkPos + glm::vec3(Chunk::SIZE, Chunk::SIZE, Chunk::SIZE))) {
      continue; // Skip chunks outside the view frustum
    }

    m_shader->setVec3("relativeChunkPos", relChunkPos);
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
  m_blockRegistryTBO.deleteBuffer();
  m_textureAtlas.cleanup();
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