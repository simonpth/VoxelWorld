#include "chunkmanager.h"
#include "engine/world.h"
#include <unordered_set>

enki::TaskScheduler chunkMeshUpdateTaskScheduler; // Assuming a global task scheduler instance is available

void ChunkManager::updateLoadedMeshes(PlayerChunkPos playerChunkPos)
{
  std::unordered_set<ChunkPosition> chunksToKeep;
  chunksToKeep.reserve(m_relativeChunkOffsets.size() * World::CHUNKHEIGHT);

  for (const auto &offset : m_relativeChunkOffsets)
  {
    for (int y = 0; y < World::CHUNKHEIGHT; ++y)
    {
      ChunkPosition chunkPos(playerChunkPos.x + offset.x, y, playerChunkPos.z + offset.z);
      chunksToKeep.insert(chunkPos);

      auto [it, inserted] = m_chunkMeshes.try_emplace(chunkPos, nullptr);
      if (inserted)
      {
        auto mesh = std::make_shared<ChunkMesh>();
        mesh->initialize();
        mesh->requestUpdate();
        
        mesh->updateVertices();

        it->second = std::move(mesh);
      }
    }
  }

  for (auto it = m_chunkMeshes.begin(); it != m_chunkMeshes.end();)
  {
    it = chunksToKeep.contains(it->first)
             ? std::next(it)
             : m_chunkMeshes.erase(it);
  }
}

void ChunkManager::setRenderDistance(int distance)
{
  m_relativeChunkOffsets.clear();
  m_relativeChunkOffsets.reserve((2 * distance + 1) * (2 * distance + 1) * (2 * distance + 1));
    for (int z = -distance; z <= distance; ++z)
    {
      for (int x = -distance; x <= distance; ++x)
      {
        m_relativeChunkOffsets.push_back(ChunkPosition(x, 0, z));
      }
    }
  m_relativeChunkOffsets.shrink_to_fit();
}