#include "chunkmeshing.h"

std::unique_ptr<ChunkMeshingData> ChunkMeshing::requestChunkMeshingData(World *world, const ChunkPosition &pos) {
  // generate needed chunks if they don't exist
  world->loadOrGenerateChunk(pos);
  world->loadOrGenerateChunk(pos + ChunkPosition(1, 0, 0));
  world->loadOrGenerateChunk(pos + ChunkPosition(0, 1, 0));
  world->loadOrGenerateChunk(pos + ChunkPosition(0, 0, 1));
  world->loadOrGenerateChunk(pos + ChunkPosition(-1, 0, 0));
  world->loadOrGenerateChunk(pos + ChunkPosition(0, -1, 0));
  world->loadOrGenerateChunk(pos + ChunkPosition(0, 0, -1));

  // Now we can safely read the chunk data
  auto meshingData = std::make_unique<ChunkMeshingData>();

  {
    ChunkReadHandle chunkReadHandle = world->getChunkRead(pos);
    meshingData->chunk = *chunkReadHandle.chunk;
  }

  // Fill in the solid masks based on the chunk's blocks
  auto chunk = &meshingData->chunk;
  for (int x = 0; x < Chunk::SIZE; ++x) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        if (chunk->block(x, y, z).isSolid()) {
          meshingData->setSolidMask(x, y, z);
        }
      }
    }
  }

  // Masks are 34x34x34 to account for neighboring blocks, so we need to check
  // adjacent chunks
  // Check +X neighbor
  if (auto chunkReadHandle = world->getChunkRead(pos + ChunkPosition(1, 0, 0))) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        if (chunkReadHandle.chunk->block(0, y, z).isSolid()) {
          meshingData->setSolidMask(Chunk::SIZE, y, z); // +X face
        }
      }
    }
  }
  // Check +Y neighbor
  if (auto chunkReadHandle = world->getChunkRead(pos + ChunkPosition(0, 1, 0))) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      for (int x = 0; x < Chunk::SIZE; ++x) {
        if (chunkReadHandle.chunk->block(x, 0, z).isSolid()) {
          meshingData->setSolidMask(x, Chunk::SIZE, z); // +Y face
        }
      }
    }
  }
  // Check +Z neighbor
  if (auto chunkReadHandle = world->getChunkRead(pos + ChunkPosition(0, 0, 1))) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int x = 0; x < Chunk::SIZE; ++x) {
        if (chunkReadHandle.chunk->block(x, y, 0).isSolid()) {
          meshingData->setSolidMask(x, y, Chunk::SIZE); // +Z face
        }
      }
    }
  }
  // Check -X neighbor
  if (auto chunkReadHandle = world->getChunkRead(pos + ChunkPosition(-1, 0, 0))) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int z = 0; z < Chunk::SIZE; ++z) {
        if (chunkReadHandle.chunk->block(Chunk::SIZE - 1, y, z).isSolid()) {
          meshingData->setSolidMask(-1, y, z); // -X face
        }
      }
    }
  }

  // Check -Y neighbor
  if (auto chunkReadHandle = world->getChunkRead(pos + ChunkPosition(0, -1, 0))) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      for (int x = 0; x < Chunk::SIZE; ++x) {
        if (chunkReadHandle.chunk->block(x, Chunk::SIZE - 1, z).isSolid()) {
          meshingData->setSolidMask(x, -1, z); // -Y face
        }
      }
    }
  }
  // Check -Z neighbor
  if (auto chunkReadHandle = world->getChunkRead(pos + ChunkPosition(0, 0, -1))) {
    for (int y = 0; y < Chunk::SIZE; ++y) {
      for (int x = 0; x < Chunk::SIZE; ++x) {
        if (chunkReadHandle.chunk->block(x, y, Chunk::SIZE - 1).isSolid()) {
          meshingData->setSolidMask(x, y, -1); // -Z face
        }
      }
    }
  }

  return std::move(meshingData);
}

uint64_t generateVertex(int id, int x, int y, int z, int width, int height, int rotation) {
  return ((static_cast<uint64_t>(id) & 0xFFFF) << 48) |
         ((static_cast<uint64_t>(x) & 0xFF) << 40) |
         ((static_cast<uint64_t>(y) & 0xFF) << 32) |
         ((static_cast<uint64_t>(z) & 0xFF) << 24) |
         ((static_cast<uint64_t>(width) & 0xFF) << 16) |
         ((static_cast<uint64_t>(height) & 0xFF) << 8) |
         ((static_cast<uint64_t>(rotation) & 0x1F) << 3);
}

void ChunkMeshing::updateChunkVertices(std::shared_ptr<ChunkVertices> vertices, ChunkMeshingData *meshingData) {
  uint32_t calculatingVersion = vertices->incrementCalculatingVersion();

  Chunk *chunk = &meshingData->chunk;

  std::vector<uint64_t> faceVertices[6]; // Temporary storage for vertices of each face direction

  // X face
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int z = 0; z < Chunk::SIZE; ++z) {
      int index = y * Chunk::SIZE + z;
      uint64_t mask = meshingData->xSolidMasks[index];
      uint64_t maskP = mask & ~(mask >> 1);
      uint64_t maskN = mask & ~(mask << 1);

      while (maskP != 0) {
        int x = std::countr_zero(maskP) - 1;
        if (x >= 0 && x < Chunk::SIZE)
          faceVertices[0].push_back(generateVertex(chunk->block(x, y, z).id,
                                                   x * 8, y * 8, z * 8, 8, 8, 0));
        maskP &= (maskP - 1);
      }
      while (maskN != 0) {
        int x = std::countr_zero(maskN) - 1;
        if (x >= 0 && x < Chunk::SIZE)
          faceVertices[3].push_back(generateVertex(chunk->block(x, y, z).id,
                                                   x * 8, y * 8, z * 8, 8, 8, 3));
        maskN &= (maskN - 1);
      }
    }
  }
  // Y face
  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      int index = z * Chunk::SIZE + x;
      uint64_t mask = meshingData->ySolidMasks[index];
      uint64_t maskP = mask & ~(mask >> 1);
      uint64_t maskN = mask & ~(mask << 1);

      while (maskP != 0) {
        int y = std::countr_zero(maskP) - 1;
        if (y >= 0 && y < Chunk::SIZE)
          faceVertices[1].push_back(generateVertex(chunk->block(x, y, z).id,
                                                   x * 8, y * 8, z * 8, 8, 8, 1));
        maskP &= (maskP - 1);
      }
      while (maskN != 0) {
        int y = std::countr_zero(maskN) - 1;
        if (y >= 0 && y < Chunk::SIZE)
          faceVertices[4].push_back(generateVertex(chunk->block(x, y, z).id,
                                                   x * 8, y * 8, z * 8, 8, 8, 4));
        maskN &= (maskN - 1);
      }
    }
  }
  // Z face
  for (int y = 0; y < Chunk::SIZE; ++y) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      int index = y * Chunk::SIZE + x;
      uint64_t mask = meshingData->zSolidMasks[index];
      uint64_t maskP = mask & ~(mask >> 1);
      uint64_t maskN = mask & ~(mask << 1);

      while (maskP != 0) {
        int z = std::countr_zero(maskP) - 1;
        if (z >= 0 && z < Chunk::SIZE)
          faceVertices[2].push_back(generateVertex(chunk->block(x, y, z).id,
                                                   x * 8, y * 8, z * 8, 8, 8, 2));
        maskP &= (maskP - 1);
      }
      while (maskN != 0) {
        int z = std::countr_zero(maskN) - 1;
        if (z >= 0 && z < Chunk::SIZE)
          faceVertices[5].push_back(generateVertex(chunk->block(x, y, z).id,
                                                   x * 8, y * 8, z * 8, 8, 8, 5));
        maskN &= (maskN - 1);
      }
    }
  }
  vertices->setVertices(faceVertices, calculatingVersion);
}