#include "chunkmanager.h"

#include <unordered_set>

#include "engine/enginecontext.h"

void ChunkManager::updateLoadedMeshes(PlayerChunkPos playerChunkPos)
{
  std::unordered_set<ChunkPosition> chunksToRemove;
  chunksToRemove.reserve(m_relativeChunkOffsets.size() * World::CHUNKHEIGHT);

  for (const auto &[chunkPos, _] : m_chunkVertices)
  {
    chunksToRemove.insert(chunkPos);
  }

  for (const auto &offset : m_relativeChunkOffsets)
  {
    for (int y = 0; y < World::CHUNKHEIGHT; ++y)
    {
      ChunkPosition chunkPos(playerChunkPos.x + offset.x, y, playerChunkPos.z + offset.z);
      chunksToRemove.erase(chunkPos);

      m_chunkVerticesMutex.lock();
      if (!m_chunkVertices.contains(chunkPos))
      {
        auto vertices = std::make_shared<ChunkVertices>();
        m_chunkVertices[chunkPos] = vertices;
        m_chunkVerticesMutex.unlock();

        auto meshingThread = std::thread([chunkPos, vertices]()
                    {
                    if (auto world = EngineContext::instance().engine()->world().lock())
                    {
                      // Generate meshing data and vertices in a separate thread to avoid blocking the main thread
                      auto meshingData = ChunkMeshing::requestChunkMeshingData(world.get(), chunkPos);
                      ChunkMeshing::updateChunkVertices(vertices, meshingData.get());
                    } });
        meshingThread.detach();
      }
      else
      {
        m_chunkVerticesMutex.unlock();
      }
    }
  }

  m_chunkVerticesMutex.lock();
  for (const auto &chunkPos : chunksToRemove)
  {
    m_chunkVertices.erase(chunkPos);
  }
  m_chunkVerticesMutex.unlock();
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