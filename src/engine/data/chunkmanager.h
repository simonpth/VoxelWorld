#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include "engine/algorithm/chunkmeshing.h"
#include "playerchunkpos.h"
#include "world.h"
#include <memory>
#include <mutex>
#include <unordered_map>

#include <taskflow/taskflow.hpp>

#include <glm/glm.hpp>

struct LoadedChunkUpdate {
  std::vector<std::pair<ChunkPosition, std::shared_ptr<ChunkVertices>>> chunksToLoad;
  std::vector<ChunkPosition> chunksToUnload;
};

struct LoadedChunkUpdatesReadHandle {
  std::queue<LoadedChunkUpdate> *loadedChunkUpdates;

  LoadedChunkUpdatesReadHandle(std::queue<LoadedChunkUpdate> *loadedChunkUpdates, std::mutex &mutex)
      : loadedChunkUpdates(loadedChunkUpdates), lock(mutex) {}

private:
  std::lock_guard<std::mutex> lock;
};

class ChunkManager {
public:
  ChunkManager();
  ~ChunkManager();

  void updateLoadedMeshes(PlayerChunkPos playerChunkPos);
  LoadedChunkUpdatesReadHandle getLoadedChunkUpdates() {
    return LoadedChunkUpdatesReadHandle(&m_loadedChunkUpdates, m_loadedChunkUpdatesMutex);
  }

  void setBlockAndUpdate(glm::ivec3 worldPos, Block block);

  void setRenderDistance(int distance);

  int loadedChunkVersion() const { return m_loadedChunkVersion.load(); }

  int renderDistance() const { return m_renderDistance; }
private:
  void updateChunkAsync(const ChunkPosition &chunkPos, std::shared_ptr<ChunkVertices> vertices = nullptr);

  std::vector<ChunkPosition> m_relativeChunkOffsets;
  std::shared_mutex m_relativeOffsetsMutex;
  std::unordered_map<ChunkPosition, std::shared_ptr<ChunkVertices>> m_chunkVertices;
  std::shared_mutex m_chunkVerticesMutex;

  tf::Executor m_executor;

  std::atomic<uint32_t> m_loadedChunkVersion = 0; // Incremented every time chunks are loaded/unloaded

  std::queue<LoadedChunkUpdate> m_loadedChunkUpdates;
  std::mutex m_loadedChunkUpdatesMutex;

  int m_renderDistance = -1; // -1 = not set yet
};

#endif // CHUNKMANAGER_H