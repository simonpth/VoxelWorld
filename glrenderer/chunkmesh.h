#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "engine/chunk.h"

#include <atomic>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QReadWriteLock>
#include <vector>

class ChunkMesh {
public:
  ChunkMesh();

  bool isReady() const { return m_ready.load(); }
  bool isDirty() const { return m_dirty.load(); }

  void setup();

  void setChunkPosition(const ChunkPosition &pos) { m_chunkPosition = pos; }
  ChunkPosition chunkPosition() const { return m_chunkPosition; }

  void updateMesh();

  void render();

private:
  QReadWriteLock m_blocksVBOLock;
  QOpenGLBuffer m_blocksVBO;
  std::vector<uint32_t> m_blockVertices;
  QOpenGLVertexArrayObject m_blocksVAO;

  std::atomic<bool> m_ready = false;
  std::atomic<bool> m_dirty = true;

  ChunkPosition m_chunkPosition;
};

#endif // CHUNKMESH_H