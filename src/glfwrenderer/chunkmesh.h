#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "glad/glad.h"

class ChunkMesh
{
public:
  ChunkMesh();
  ~ChunkMesh();

private:
  GLuint m_vbo;
  GLuint m_vao;
};

#endif // CHUNKMESH_H