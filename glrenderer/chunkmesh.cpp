#define GL_SILENCE_DEPRECATION

#include "chunkmesh.h"

#include <QDebug>
#include <QThreadPool>
#include <cstdint>

ChunkMesh::ChunkMesh() : QOpenGLFunctions(QOpenGLContext::currentContext()) {}

void ChunkMesh::setup() {
  m_vao.create();
  m_vao.bind();

  m_vbo.create();
  m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo.bind();

  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, sizeof(uint64_t), (void *)0);
  glVertexAttribDivisor(0, 1);

  m_vao.release();
}

uint64_t generateVertex(int id, int x, int y, int z, int width, int height, int rotation) {
  return ((static_cast<uint64_t>(id) & 0xFFFF) << 48) |
         ((static_cast<uint64_t>(x) & 0xFF) << 40) |
         ((static_cast<uint64_t>(y) & 0xFF) << 32) |
         ((static_cast<uint64_t>(z) & 0xFF) << 24) |
         ((static_cast<uint64_t>(width) & 0xFF) << 16) |
         ((static_cast<uint64_t>(height) & 0xFF) << 8) |
         ((static_cast<uint64_t>(rotation) & 0x1F) << 3);
}

void ChunkMesh::updateMeshAsync() {
  QThreadPool::globalInstance()->start([this]() {
    // Generate dummy vertex data for testing

    std::vector<uint64_t> newVertices;

    newVertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 0));
    newVertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 1));
    newVertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 2));
    newVertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 3));
    newVertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 4));
    newVertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 5));

    QWriteLocker locker(&m_verticesLock);
    m_vertices.swap(newVertices);
    m_newAllocRequired.store(true);

    m_ready.store(true);
  });
}

void ChunkMesh::render() {
  if (!isReady())
    return;

  if(m_newAllocRequired.load()) {
    m_vbo.bind();
    QReadLocker locker(&m_verticesLock);
    m_vbo.allocate(m_vertices.data(), static_cast<int>(m_vertices.size() * sizeof(uint64_t)));
    m_uploadedVertexCount = m_vertices.size();
    m_newAllocRequired.store(false);
  }

  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, sizeof(uint64_t), (void *)0);
  glVertexAttribDivisor(0, 1);

  m_vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, m_uploadedVertexCount, 4);
  m_vao.release();
}