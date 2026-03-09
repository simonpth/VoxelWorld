#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "engine/chunk.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QReadWriteLock>
#include <atomic>

class ChunkMesh : protected QOpenGLFunctions {
public:
  ChunkMesh();

  bool isReady() const { return m_ready.load(); }

  void setup();

  void setChunkPosition(const ChunkPosition &pos) { m_chunkPosition = pos; }
  ChunkPosition chunkPosition() const { return m_chunkPosition; }

  void updateMeshAsync();

  void render();

private:
  QReadWriteLock m_vboLock;
  QOpenGLBuffer m_vbo;
  int m_vertexCount = 0;
  QOpenGLVertexArrayObject m_vao;

  std::atomic<bool> m_ready = false;

  ChunkPosition m_chunkPosition;
};

#endif // CHUNKMESH_H