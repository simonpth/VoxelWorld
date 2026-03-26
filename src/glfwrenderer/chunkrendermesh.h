#ifndef CHUNKRENDERMESH_H
#define CHUNKRENDERMESH_H

#include "engine/data/chunkvertices.h"

#include "glad/glad.h"

#include <shared_mutex>
#include <vector>
#include <mutex>
#include <memory>

class ChunkRenderMesh
{
public:
  ChunkRenderMesh();
  ~ChunkRenderMesh();

  void setChunkVertices(std::shared_ptr<ChunkVertices> vertices) { m_chunkVertices = vertices; }

  void initialize();
  void uploadVerticesIfNeeded();

  void render();

private:
  GLuint m_vbo;
  GLuint m_vao;

  std::shared_ptr<ChunkVertices> m_chunkVertices;
  uint32_t m_currentVerticesSize = 0; // Number of vertices currently stored in the VBO
  uint32_t m_uploadedVersion = 0; // Version of vertices currently uploaded to GPU
};

#endif // CHUNKRENDERMESH_H