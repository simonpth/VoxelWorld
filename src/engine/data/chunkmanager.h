#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include "engine/algorithm/chunkmeshing.h"
#include "world.h"
#include "playerchunkpos.h"

class ChunkManager
{
public:
  ChunkManager() = default;

  void updateLoadedMeshes(PlayerChunkPos playerChunkPos);

  void setRenderDistance(int distance);

  // Only after lockChunkVertices()
  const std::unordered_map<ChunkPosition, std::shared_ptr<ChunkVertices>> &chunkVertices() const { return m_chunkVertices; }

  void lockChunkVertices() { m_chunkVerticesMutex.lock(); } // Before rendering
  void unlockChunkVertices() { m_chunkVerticesMutex.unlock(); } // After rendering
private:
  std::vector<ChunkPosition> m_relativeChunkOffsets;
  std::unordered_map<ChunkPosition, std::shared_ptr<ChunkVertices>> m_chunkVertices;
  std::mutex m_chunkVerticesMutex;
};

#endif // CHUNKMANAGER_H