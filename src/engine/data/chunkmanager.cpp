#include "chunkmanager.h"

#include <unordered_set>

#include "engine/enginecontext.h"

ChunkManager::ChunkManager() {
}

ChunkManager::~ChunkManager() {
}

void ChunkManager::updateLoadedMeshes(PlayerChunkPos playerChunkPos) {
  LoadedChunkUpdate update;
  update.chunksToLoad.reserve(32);
  update.chunksToUnload.reserve(32);

  std::unordered_set<ChunkPosition> positions;
  positions.reserve(m_relativeChunkOffsets.size());
  {
    std::shared_lock lock(m_relativeOffsetsMutex);
    for (const ChunkPosition &offset : m_relativeChunkOffsets) {
      ChunkPosition pos = ChunkPosition(playerChunkPos.x + offset.x, offset.y, playerChunkPos.z + offset.z);
      positions.insert(pos);
    }
  }

  std::shared_lock lock(m_chunkVerticesMutex);
  for (const auto &[chunkPos, vertices] : m_chunkVertices) {
    if (!positions.contains(chunkPos)) {
      update.chunksToUnload.push_back(chunkPos);
    }
  }

  for (const ChunkPosition &pos : positions) {
    if (!m_chunkVertices.contains(pos)) {
      update.chunksToLoad.push_back(std::make_pair(pos, std::make_shared<ChunkVertices>()));
    }
  }

  if (!update.chunksToLoad.empty() || !update.chunksToUnload.empty()) {
    for (const auto &[pos, vertices] : update.chunksToLoad) {
      m_chunkVertices[pos] = vertices;
      updateChunkAsync(pos, vertices);
    }
    for (const ChunkPosition &pos : update.chunksToUnload) {
      m_chunkVertices.erase(pos);
    }

    std::lock_guard lock(m_loadedChunkUpdatesMutex);
    m_loadedChunkUpdates.push(std::move(update));
    m_loadedChunkVersion.fetch_add(1);
  }
}

void ChunkManager::setBlockAndUpdate(glm::ivec3 worldPos, Block block) {
  auto [chunkPos, pos] = World::worldPosToChunkAndBlockPos(worldPos);

  auto world = EngineContext::instance().engine()->world();
  world->setBlock(worldPos, block);

  // Update the chunk containing the block and its neighbors if the block is on the edge of the chunk
  updateChunkAsync(chunkPos);
  if (pos.x == 0) {
    // Update -X neighbor
    ChunkPosition neighborPos = chunkPos + ChunkPosition(-1, 0, 0);
    updateChunkAsync(neighborPos);
  } else if (pos.x == Chunk::SIZE - 1) {
    // Update +X neighbor
    ChunkPosition neighborPos = chunkPos + ChunkPosition(1, 0, 0);
    updateChunkAsync(neighborPos);
  }
  if (pos.y == 0) {
    // Update -Y neighbor
    ChunkPosition neighborPos = chunkPos + ChunkPosition(0, -1, 0);
    updateChunkAsync(neighborPos);
  } else if (pos.y == Chunk::SIZE - 1) {
    // Update +Y neighbor
    ChunkPosition neighborPos = chunkPos + ChunkPosition(0, 1, 0);
    updateChunkAsync(neighborPos);
  }
  if (pos.z == 0) {
    // Update -Z neighbor
    ChunkPosition neighborPos = chunkPos + ChunkPosition(0, 0, -1);
    updateChunkAsync(neighborPos);
  } else if (pos.z == Chunk::SIZE - 1) {
    // Update +Z neighbor
    ChunkPosition neighborPos = chunkPos + ChunkPosition(0, 0, 1);
    updateChunkAsync(neighborPos);
  }
}

void ChunkManager::setRenderDistance(int distance) {
  std::unique_lock lock(m_relativeOffsetsMutex);
  m_relativeChunkOffsets.clear();
  m_relativeChunkOffsets.reserve((2 * distance + 1) * (2 * distance + 1) * World::CHUNKHEIGHT);
  for (int z = -distance; z <= distance; ++z) {
    for (int x = -distance; x <= distance; ++x) {
      int distanceAndOne = distance + 1;
      if(x*x + z*z >= distanceAndOne * distanceAndOne) {
        continue; // Skip chunks outside the circular render distance
      }
      for (int y = 0; y < World::CHUNKHEIGHT; ++y) {
        m_relativeChunkOffsets.push_back(ChunkPosition(x, y, z));
      }
    }
  }
  m_relativeChunkOffsets.shrink_to_fit();
  m_renderDistance = distance;
}

void ChunkManager::updateChunkAsync(const ChunkPosition &chunkPos, std::shared_ptr<ChunkVertices> vertices) {
  if (!vertices) {
    auto verticesIt = m_chunkVertices.find(chunkPos);
    if (verticesIt != m_chunkVertices.end()) {
      vertices = verticesIt->second;
    }
  }
  if (vertices) {
    m_executor.silent_async([chunkPos, vertices]() {
      auto world = EngineContext::instance().engine()->world();
      auto meshingData = ChunkMeshing::requestChunkMeshingData(world.get(), chunkPos);
      ChunkMeshing::updateChunkVertices(vertices, meshingData.get());
    });
  }
}