#include "chunkmesh.h"

#include <GLFW/glfw3.h>

ChunkMesh::ChunkMesh()
{
  // Constructor implementation (if needed)
}

ChunkMesh::~ChunkMesh()
{
  // check for active OpenGL context before deleting buffers
  if (glfwGetCurrentContext() != nullptr)
  {
    if (m_vbo != 0)
    {
      glDeleteBuffers(1, &m_vbo);
      m_vbo = 0;
    }
    if (m_vao != 0)
    {
      glDeleteVertexArrays(1, &m_vao);
      m_vao = 0;
    }
  }
}

void ChunkMesh::requestUpdate()
{
  m_needsUpdate.store(true);
}

uint64_t generateVertex(int id, int x, int y, int z, int width, int height, int rotation)
{
  return ((static_cast<uint64_t>(id) & 0xFFFF) << 48) |
         ((static_cast<uint64_t>(x) & 0xFF) << 40) |
         ((static_cast<uint64_t>(y) & 0xFF) << 32) |
         ((static_cast<uint64_t>(z) & 0xFF) << 24) |
         ((static_cast<uint64_t>(width) & 0xFF) << 16) |
         ((static_cast<uint64_t>(height) & 0xFF) << 8) |
         ((static_cast<uint64_t>(rotation) & 0x1F) << 3);
}

void ChunkMesh::updateVertices()
{
  std::lock_guard updateLock(m_updateMutex);
  if (!m_needsUpdate.load())
    return;
  m_needsUpdate.store(false);

  std::vector<uint64_t> newVertices;

  newVertices.push_back(generateVertex(1, 16*8, 16*8, 16*8, 8, 8, 0));
  newVertices.push_back(generateVertex(1, 16*8, 16*8, 16*8, 8, 8, 1));
  newVertices.push_back(generateVertex(1, 16*8, 16*8, 16*8, 8, 8, 2));
  newVertices.push_back(generateVertex(1, 16*8, 16*8, 16*8, 8, 8, 3));
  newVertices.push_back(generateVertex(1, 16*8, 16*8, 16*8, 8, 8, 4));
  newVertices.push_back(generateVertex(1, 16*8, 16*8, 16*8, 8, 8, 5));

  std::unique_lock verticesLock(m_verticesMutex);
  m_vertices.swap(newVertices);
  m_needsUpload = true;
}

void ChunkMesh::initialize()
{
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

  glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, sizeof(uint64_t), (void *)0);
  glVertexAttribDivisor(0, 1);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

void ChunkMesh::uploadVertices()
{
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  std::shared_lock verticesLock(m_verticesMutex);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(uint64_t),
               m_vertices.data(), GL_STATIC_DRAW);
}

void ChunkMesh::render()
{
  std::shared_lock verticesLock(m_verticesMutex);

  if (m_needsUpload)
  {
    uploadVertices();
    m_needsUpload = false;
  }

  if (m_vertices.empty())
    return;

  glBindVertexArray(m_vao);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_vertices.size());
  glBindVertexArray(0);
}