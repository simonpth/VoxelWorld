#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "glad/glad.h"
#include <vector>
#include <shared_mutex>

class ChunkMesh
{
public:
  ChunkMesh();
  ~ChunkMesh();

  void updateVertices();

  void initialize();
  void uploadVertices();

  void render();

private:
  GLuint m_vbo;
  GLuint m_vao;

  std::shared_mutex m_verticesMutex;
  std::vector<uint64_t> m_vertices;
  bool m_needsUpload = false;
};

#endif // CHUNKMESH_H