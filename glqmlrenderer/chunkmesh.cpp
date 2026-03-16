#define GL_SILENCE_DEPRECATION
#include "chunkmesh.h"
#include <QDebug>
#include <QThreadPool>
#include <bit>
#include <cstdint>

ChunkMesh::ChunkMesh() : QOpenGLFunctions(QOpenGLContext::currentContext()) {}

ChunkMesh::~ChunkMesh() {
  m_vao.destroy();
  m_vbo.destroy();
}

void ChunkMesh::setup() {
  m_vao.create();
  m_vao.bind();

  m_vbo.create();
  m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo.bind();

  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, sizeof(uint64_t), (void *)0);
  glVertexAttribDivisor(0, 1);

  m_vao.release();
}

uint64_t generateVertex(int id, int x, int y, int z, int width, int height,
                        int rotation) {
  return ((static_cast<uint64_t>(id) & 0xFFFF) << 48) |
         ((static_cast<uint64_t>(x) & 0xFF) << 40) |
         ((static_cast<uint64_t>(y) & 0xFF) << 32) |
         ((static_cast<uint64_t>(z) & 0xFF) << 24) |
         ((static_cast<uint64_t>(width) & 0xFF) << 16) |
         ((static_cast<uint64_t>(height) & 0xFF) << 8) |
         ((static_cast<uint64_t>(rotation) & 0x1F) << 3);
}

void ChunkMesh::updateMeshAsync(std::shared_ptr<World> world) {
  QThreadPool::globalInstance()->start([this, world]() {
    m_uses.fetch_add(1);
    QMutexLocker updateLocker(&m_updateMeshMutex);

    std::vector<uint64_t> newVertices;

    auto renderData = world->requestChunkRenderData(m_chunkPosition);
    Chunk *chunk = &renderData->chunk;

    // X face
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        int index = y * Chunk::SIZE + z;
        uint64_t mask = renderData->xSolidMasks[index];
        uint64_t maskP = mask & ~(mask >> 1);
        uint64_t maskN = mask & ~(mask << 1);

        while (maskP != 0) {
          int x = std::countr_zero(maskP) - 1;
          if (x >= 0 && x < Chunk::SIZE) // Only generate vertices for blocks
                                         // within the chunk
            newVertices.push_back(generateVertex(chunk->block(x, y, z).id,
                                                 x * 8, y * 8, z * 8, 8, 8, 0));
          maskP &= (maskP - 1);
        }
        while (maskN != 0) {
          int x = std::countr_zero(maskN) - 1;
          if (x >= 0 && x < Chunk::SIZE) // Only generate vertices for blocks
                                         // within the chunk
            newVertices.push_back(generateVertex(chunk->block(x, y, z).id,
                                                 x * 8, y * 8, z * 8, 8, 8, 3));
          maskN &= (maskN - 1);
        }
      }
    }
    // Y face
    for (int z = 0; z < Chunk::SIZE; ++z) {
      for (int x = 0; x < Chunk::SIZE; ++x) {
        int index = z * Chunk::SIZE + x;
        uint64_t mask = renderData->ySolidMasks[index];
        uint64_t maskP = mask & ~(mask >> 1);
        uint64_t maskN = mask & ~(mask << 1);

        while (maskP != 0) {
          int y = std::countr_zero(maskP) - 1;
          if (y >= 0 && y < Chunk::SIZE) // Only generate vertices for blocks
                                         // within the chunk
            newVertices.push_back(generateVertex(chunk->block(x, y, z).id,
                                                 x * 8, y * 8, z * 8, 8, 8, 1));
          maskP &= (maskP - 1);
        }
        while (maskN != 0) {
          int y = std::countr_zero(maskN) - 1;
          if (y >= 0 && y < Chunk::SIZE) // Only generate vertices for blocks
                                         // within the chunk
            newVertices.push_back(generateVertex(chunk->block(x, y, z).id,
                                                 x * 8, y * 8, z * 8, 8, 8, 4));
          maskN &= (maskN - 1);
        }
      }
    }
    // Z face
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int x = 0; x < Chunk::SIZE; ++x) {
        int index = y * Chunk::SIZE + x;
        uint64_t mask = renderData->zSolidMasks[index];
        uint64_t maskP = mask & ~(mask >> 1);
        uint64_t maskN = mask & ~(mask << 1);

        while (maskP != 0) {
          int z = std::countr_zero(maskP) - 1;
          if (z >= 0 && z < Chunk::SIZE) // Only generate vertices for blocks
                                         // within the chunk
            newVertices.push_back(generateVertex(chunk->block(x, y, z).id,
                                                 x * 8, y * 8, z * 8, 8, 8, 2));
          maskP &= (maskP - 1);
        }
        while (maskN != 0) {
          int z = std::countr_zero(maskN) - 1;
          if (z >= 0 && z < Chunk::SIZE) // Only generate vertices for blocks
                                         // within the chunk
            newVertices.push_back(generateVertex(chunk->block(x, y, z).id,
                                                 x * 8, y * 8, z * 8, 8, 8, 5));
          maskN &= (maskN - 1);
        }
      }
    }

    QWriteLocker verticesLocker(&m_verticesLock);
    m_vertices.swap(newVertices);
    m_newAllocRequired.store(true);

    m_ready.store(true);
    updateLocker.unlock();
    verticesLocker.unlock();
    m_uses.fetch_sub(1);
  });
}

void ChunkMesh::render() {
  m_uses.fetch_add(1);
  if (!isReady()) {
    m_uses.fetch_sub(1);
    return;
  }

  if (m_newAllocRequired.load()) {
    m_vbo.bind();
    QReadLocker locker(&m_verticesLock);
    m_vbo.allocate(m_vertices.data(),
                   static_cast<int>(m_vertices.size() * sizeof(uint64_t)));
    m_uploadedVertexCount = m_vertices.size();
    m_newAllocRequired.store(false);
  }

  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, sizeof(uint64_t), (void *)0);
  glVertexAttribDivisor(0, 1);

  m_vao.bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_uploadedVertexCount);
  m_vao.release();

  m_uses.fetch_sub(1);
}

void ChunkMesh::cleanupAsync(std::unique_ptr<ChunkMesh> mesh) {
  QThreadPool::globalInstance()->start([mesh = std::move(mesh)]() mutable {
    while (mesh->m_uses.load() > 0) {
      QThread::msleep(100);
    }
  });
}