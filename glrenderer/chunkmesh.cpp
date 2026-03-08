#include "chunkmesh.h"
#include <OpenGL/gl.h>
#include <cstdint>

ChunkMesh::ChunkMesh() {}

void ChunkMesh::setup() {
  m_blocksVAO.create();
  m_blocksVAO.bind();

  m_blocksVBO.create();
  m_blocksVBO.bind();

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(uint32_t), (void *)0);

  m_blocksVAO.release();
}

void ChunkMesh::updateMesh() {

  m_ready.store(true);
  m_dirty.store(false);
}

void ChunkMesh::render() {
  if (!isReady())
    return;

  m_blocksVAO.bind();
  // glDrawArrays or glDrawElements based on how you set up your VBO/VAO
  m_blocksVAO.release();
}