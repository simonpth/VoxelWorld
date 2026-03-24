#include "chunkgeneration.h"
#include "chunk.h"
#include "world.h"

#include <cmath>

// sonnet 4.6
int perlinNoise(int x, int z, int seed = 0) {
    auto fade = [](double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    };

    auto lerp = [](double a, double b, double t) {
        return a + t * (b - a);
    };

    auto grad = [](int hash, double x, double z) {
        int h = hash & 3;
        double u = h < 2 ? x : z;
        double v = h < 2 ? z : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    };

    auto hashCoord = [](int ix, int iz, int seed) {
        int n = seed ^ (ix * 1619) ^ (iz * 31337);
        n = (n << 13) ^ n;
        return (int)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) & 255;
    };

    auto sampleNoise = [&](double fx, double fz, int s) {
        int xi = (int)std::floor(fx);
        int zi = (int)std::floor(fz);
        double xf = fx - xi;
        double zf = fz - zi;

        double u = fade(xf);
        double v = fade(zf);

        int aa = hashCoord(xi,     zi,     s);
        int ab = hashCoord(xi,     zi + 1, s);
        int ba = hashCoord(xi + 1, zi,     s);
        int bb = hashCoord(xi + 1, zi + 1, s);

        double x1 = lerp(grad(aa, xf,     zf    ), grad(ba, xf - 1, zf    ), u);
        double x2 = lerp(grad(ab, xf,     zf - 1), grad(bb, xf - 1, zf - 1), u);
        return lerp(x1, x2, v);
    };

    // Octave settings
    const int    octaves    = 4;
    const double lacunarity = 2.0;   // frequency multiplier per octave
    const double persistence = 0.5;  // amplitude multiplier per octave

    double noise     = 0.0;
    double amplitude = 1.0;
    double frequency = 0.007;
    double maxValue  = 0.0;

    for (int i = 0; i < octaves; i++) {
        noise    += sampleNoise(x * frequency, z * frequency, seed + i * 67) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    noise /= maxValue; // normalize to [-1, 1]

    return (int)std::round(64 + (noise + 1) / 2.0 * 128);
}

std::unique_ptr<Chunk>
ChunkGeneration::generateChunk(const ChunkPosition &pos) {
  auto chunk = std::make_unique<Chunk>();

  // Simple terrain generation: create a flat terrain at y=128
  if (pos.y < 0 || pos.y >= World::CHUNKHEIGHT) {
    return chunk; // empty chunk
  }

  for (int z = 0; z < Chunk::SIZE; ++z) {
    for (int x = 0; x < Chunk::SIZE; ++x) {
      int worldX = pos.x * Chunk::SIZE + x;
      int worldZ = pos.z * Chunk::SIZE + z;
      int height = perlinNoise(worldX, worldZ);

      for (int y = 0; y < height - pos.y * Chunk::SIZE && y < Chunk::SIZE; ++y) {
        chunk->setBlock(x, y, z, Block(2)); // Set block type 1 (e.g., stone)
      }
    }
  }

  return std::move(chunk);
}