#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "engine/chunk.h"
#include "engine/world.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QReadWriteLock>
#include <atomic>
#include <memory>
#include <vector>
#include <cstdint>

class ChunkMesh : protected QOpenGLFunctions {
public:
  ChunkMesh();

  bool isReady() const { return m_ready.load(); }

  void setup();

  void setChunkPosition(const ChunkPosition &pos) { m_chunkPosition = pos; }
  ChunkPosition chunkPosition() const { return m_chunkPosition; }

  void updateMeshAsync(std::shared_ptr<World> world);

  void render();

  static void cleanupAsync(std::unique_ptr<ChunkMesh> mesh);

private:
  QOpenGLBuffer m_vbo;
  QReadWriteLock m_verticesLock;
  std::vector<uint64_t> m_vertices;
  size_t m_uploadedVertexCount = 0;

  QOpenGLVertexArrayObject m_vao;

  std::atomic<bool> m_newAllocRequired = false;
  std::atomic<bool> m_ready = false;

  ChunkPosition m_chunkPosition;

  QMutex m_updateMeshMutex;
  std::atomic<int> m_uses = 0;
};

#endif // CHUNKMESH_H