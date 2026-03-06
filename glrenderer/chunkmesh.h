#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "engine/chunk.h"

#include <atomic>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class ChunkMesh {
public:
  ChunkMesh();

  bool isReady() const { return m_ready.load(); }
  bool isDirty() const { return m_dirty.load(); }

private:
  QOpenGLBuffer m_vbo;
  QOpenGLVertexArrayObject m_vao;

  std::atomic<bool> m_ready = false;
  std::atomic<bool> m_dirty = true;
};

#endif // CHUNKMESH_H