#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include "engine/algorithm/chunkmeshing.h"
#include "world.h"
#include "playerchunkpos.h"

#include <taskflow/taskflow.hpp>

#include <glm/glm.hpp>

class ChunkManager
{
public:
  ChunkManager();
  ~ChunkManager();

  void updateLoadedMeshes(PlayerChunkPos playerChunkPos);

  void setBlockAndUpdate(glm::ivec3 worldPos, Block block);

  void setRenderDistance(int distance);

  // Only after lockChunkVertices()
  const std::unordered_map<ChunkPosition, std::shared_ptr<ChunkVertices>> &chunkVertices() const { return m_chunkVertices; }

  void lockChunkVertices() { m_chunkVerticesMutex.lock(); } // Before rendering
  void unlockChunkVertices() { m_chunkVerticesMutex.unlock(); } // After rendering
private:
  void updateChunkAsync(const ChunkPosition &chunkPos, std::shared_ptr<ChunkVertices> vertices = nullptr);

  std::vector<ChunkPosition> m_relativeChunkOffsets;
  std::unordered_map<ChunkPosition, std::shared_ptr<ChunkVertices>> m_chunkVertices;
  std::mutex m_chunkVerticesMutex;

  tf::Executor m_executor;
};

#endif // CHUNKMANAGER_H