#include "chunkrendermesh.h"

#include <GLFW/glfw3.h>

ChunkRenderMesh::ChunkRenderMesh()
{
  // Constructor implementation (if needed)
}

ChunkRenderMesh::~ChunkRenderMesh()
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

void ChunkRenderMesh::initialize()
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

bool ChunkRenderMesh::uploadVerticesIfNeeded()
{
  if (m_uploadedVersion != m_chunkVertices->version())
  {
    m_chunkVertices->lockShared();
    auto vertices = m_chunkVertices->vertices();

    m_currentVerticesSize = vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_currentVerticesSize * sizeof(uint64_t),
                 vertices.data(), GL_STATIC_DRAW);

    m_uploadedVersion = m_chunkVertices->version();

    m_chunkVertices->unlockShared();
    return true;
  }
  return false;
}

void ChunkRenderMesh::render()
{
  if (m_currentVerticesSize > 0)
  {
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_currentVerticesSize);
    glBindVertexArray(0);
  }
}