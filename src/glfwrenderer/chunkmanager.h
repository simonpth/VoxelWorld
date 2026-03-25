#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <memory>
#include "chunkmesh.h"
#include "engine/chunk.h"
#include "engine/playerchunkpos.h"

class ChunkManager
{
public:
  ChunkManager() = default;

  void updateLoadedMeshes(PlayerChunkPos playerChunkPos);

  void setRenderDistance(int distance);

  const std::unordered_map<ChunkPosition, std::shared_ptr<ChunkMesh>> &chunkMeshes() const { return m_chunkMeshes; }

private:
  std::vector<ChunkPosition> m_relativeChunkOffsets;
  std::unordered_map<ChunkPosition, std::shared_ptr<ChunkMesh>> m_chunkMeshes;
};

#endif // CHUNKMANAGER_H