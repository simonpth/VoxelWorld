#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "glad/glad.h"

#include <shared_mutex>
#include <vector>

#include "TaskScheduler.h"

class ChunkMesh
{
public:
  ChunkMesh();
  ~ChunkMesh();

  void requestUpdate();

  void updateVerticesIfNeeded();

  void initialize();
  void uploadVertices();

  void render();

private:
  GLuint m_vbo;
  GLuint m_vao;

  std::atomic<bool> m_needsUpdate{true};

  std::shared_mutex m_verticesMutex;
  std::vector<uint64_t> m_vertices;
  bool m_needsUpload = false;
};

struct ChunkMeshUpdateVerticesTask : enki::ITaskSet
{
  std::shared_ptr<ChunkMesh> m_chunkMesh;

  ChunkMeshUpdateVerticesTask(std::shared_ptr<ChunkMesh> chunkMesh) : m_chunkMesh(chunkMesh) {}

  void ExecuteRange(enki::TaskSetPartition range, uint32_t threadnum) override
  {
    m_chunkMesh->updateVerticesIfNeeded();
  }
};

#endif // CHUNKMESH_H