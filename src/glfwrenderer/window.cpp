#include "window.h"

#include "engine/enginecontext.h"
#include "engine/constants.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Window::Window()
{
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

bool Window::initialize()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_window = glfwCreateWindow(800, 600, "Voxel World", nullptr, nullptr);
  if (m_window == nullptr)
  {
    std::println("Failed to create GLFW window");
    return false;
  }
  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(0);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::println("Failed to initialize GLAD");
    return false;
  }
  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

  m_shader = std::make_unique<Shader>("shaders/shader.vert", "shaders/shader.frag");

  return true;
}

void Window::mainLoop()
{
  // GLint chunkMVPLocation = m_shader->uniformLocation("mvp");
  // GLint relativeChunkPosLocation = m_shader->uniformLocation("relativeChunkPos");
  while (!glfwWindowShouldClose(m_window))
  {
    processInput();

    auto now = std::chrono::steady_clock::now();
    if (m_lastFrame.time_since_epoch().count() != 0)
    {
      // update player controller with the time delta since the last frame
      auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastFrame);
      m_playerController.update(m_inputState, delta);

      // update FPS counter every second
      m_frameCountSinceLastFpsUpdate++;
      auto fpsDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastFpsUpdate);
      if (fpsDelta.count() >= 1000)
      {
        int fps = static_cast<int>(m_frameCountSinceLastFpsUpdate * 1000 / fpsDelta.count());
        m_frameCountSinceLastFpsUpdate = 0;
        glfwSetWindowTitle(m_window, ("Voxel World - FPS: " + std::to_string(fps)).c_str());
        m_lastFpsUpdate = now;
      }
    }
    else
    {
      // first frame, just set the lastFrame time
      m_lastFpsUpdate = now;
    }
    m_lastFrame = now;

    // calculate the MVP matrix for the player's current position and rotation
    glm::vec3 playerPos = m_playerController.position();
    PlayerChunkPos playerChunkPos = m_playerController.currentChunk();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(
        playerPos,
        playerPos + m_playerController.front(),
        m_playerController.up());

    glm::mat4 mvp = projection * view;

    // if the player's current chunk or render distance has changed, update the list of chunks to render
    if (m_playerController.chunksToRenderDirty() && false) // DEBUG: disable chunk loading
    {
      m_playerController.setChunksToRenderDirty(false);
      // reset the dirty flag first so that we don't miss any updates
      // that happen during this function
      auto relativeOffsets = m_playerController.relativeChunkOffsets();

      std::unordered_map<ChunkPosition, bool> chunksToKeep;
      for (const auto &offset : relativeOffsets)
      {
        for (int y = 0; y < World::CHUNKHEIGHT; ++y)
        {
          auto chunkPos = ChunkPosition(playerChunkPos.x + offset.x, y,
                                        playerChunkPos.z + offset.z);

          // if the chunk isn't already in the map, create a new mesh for it
          if (m_chunkMeshes.find(chunkPos) == m_chunkMeshes.end())
          {
            auto mesh = std::make_unique<ChunkMesh>();
            mesh->initialize();
            mesh->requestUpdate();
            mesh->updateVerticesIfNeeded();
            m_chunkMeshes.emplace(chunkPos, std::move(mesh));
          }

          chunksToKeep[chunkPos] = true;
        }
      }

      // remove chunks that are no longer in the render distance
      for (auto it = m_chunkMeshes.begin(); it != m_chunkMeshes.end();)
      {
        if (chunksToKeep.find(it->first) == chunksToKeep.end())
        {
          it = m_chunkMeshes.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
    

    // render the chunks
    glClearColor(0.0f, 0.75f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->use();

    // m_shader->setMat4(chunkMVPLocation, mvp);

    for (const auto &[chunkPos, chunkMesh] : m_chunkMeshes)
    {
      float relativeX = (chunkPos.x - playerChunkPos.x) * Chunk::SIZE;
      float relativeY = (chunkPos.y - playerChunkPos.y) * Chunk::SIZE;
      float relativeZ = (chunkPos.z - playerChunkPos.z) * Chunk::SIZE;
      // m_shader->setVec3(relativeChunkPosLocation, relativeX, relativeY, relativeZ);
      chunkMesh->render();
    }

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void Window::cleanup()
{
  glfwDestroyWindow(m_window);
  m_window = nullptr;
  glfwTerminate();
}

void Window::processInput()
{
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(m_window, true);
  }

  m_inputState.moveForward = glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS;
  m_inputState.moveBackward = glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS;
  m_inputState.moveLeft = glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS;
  m_inputState.moveRight = glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS;
  m_inputState.moveUp = glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS;
  m_inputState.moveDown = glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
}