#include "chunkmanager.h"

#include <unordered_set>

#include "engine/enginecontext.h"

ChunkManager::ChunkManager() {
}

ChunkManager::~ChunkManager() {
}

void ChunkManager::updateLoadedMeshes(PlayerChunkPos playerChunkPos) {
  std::unordered_set<ChunkPosition> chunksToRemove;
  chunksToRemove.reserve(m_relativeChunkOffsets.size() * World::CHUNKHEIGHT);

  for (const auto &[chunkPos, _] : m_chunkVertices) {
    chunksToRemove.insert(chunkPos);
  }

  std::lock_guard lock(m_chunkVerticesMutex);
  for (const auto &offset : m_relativeChunkOffsets) {
    for (int y = 0; y < World::CHUNKHEIGHT; ++y) {
      ChunkPosition chunkPos(playerChunkPos.x + offset.x, y, playerChunkPos.z + offset.z);
      chunksToRemove.erase(chunkPos);

      if (!m_chunkVertices.contains(chunkPos)) {
        auto vertices = std::make_shared<ChunkVertices>();
        m_chunkVertices[chunkPos] = vertices;

        updateChunkAsync(chunkPos, vertices);
      }
    }
  }

  for (const auto &chunkPos : chunksToRemove) {
    m_chunkVertices.erase(chunkPos);
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
  m_relativeChunkOffsets.clear();
  m_relativeChunkOffsets.reserve((2 * distance + 1) * (2 * distance + 1) * (2 * distance + 1));
  for (int z = -distance; z <= distance; ++z) {
    for (int x = -distance; x <= distance; ++x) {
      m_relativeChunkOffsets.push_back(ChunkPosition(x, 0, z));
    }
  }
  m_relativeChunkOffsets.shrink_to_fit();
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