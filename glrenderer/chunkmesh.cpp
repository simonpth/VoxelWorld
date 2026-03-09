#define GL_SILENCE_DEPRECATION

#include "chunkmesh.h"

#include <QDebug>
#include <QThreadPool>
#include <cstdint>

ChunkMesh::ChunkMesh() {}

void ChunkMesh::setup() {
  m_vao.create();
  m_vao.bind();

  m_vbo.create();
  m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo.bind();

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
    std::vector<uint64_t> vertices;

    vertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 0));
    vertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 1));
    vertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 2));
    vertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 3));
    vertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 4));
    vertices.push_back(generateVertex(1, 0, 0, 0, 8, 8, 5));
    
    // Update VBO with new vertex data
    QWriteLocker locker(&m_vboLock);
    m_vbo.bind();
    m_vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(uint64_t)));
    m_vbo.release();
    m_vertexCount = static_cast<int>(vertices.size());

    m_ready.store(true);
  });
}

void ChunkMesh::render() {
  if (!isReady())
    return;

  m_vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_vertexCount);
  m_vao.release();
}