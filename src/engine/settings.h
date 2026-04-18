#ifndef SETTINGS_H
#define SETTINGS_H

#include <atomic>

class Settings {
public:
  static Settings &instance() {
    static Settings s;
    return s;
  }

  int renderDistance() const { return m_renderDistance.load(); }
  void setRenderDistance(int distance) { m_renderDistance.store(distance); }

  bool vsync() const { return m_vsync.load(); }
  void setVsync(bool enabled) { m_vsync.store(enabled); }

  float playerSpeed() const { return m_playerSpeed.load(); }
  void setPlayerSpeed(float speed) { m_playerSpeed.store(speed); }

  int warpMode() const { return m_warpMode.load(); }
  void setWarpMode(int mode) { m_warpMode.store(mode); }

  bool useTextures() const { return m_useTextures.load(); }
  void setUseTextures(bool enabled) { m_useTextures.store(enabled); }

  float textureFadeDistance() const { return m_textureFadeDistance.load(); }
  void setTextureFadeDistance(float distance) { m_textureFadeDistance.store(distance); }
  float textureFadeStrength() const { return m_textureFadeStrength.load(); }
  void setTextureFadeStrength(float strength) { m_textureFadeStrength.store(strength); }

  int worldSeed() const { return m_worldSeed.load(); }
  void setWorldSeed(int seed) { m_worldSeed.store(seed); }

  int planetSizeInChunks() const { return m_planetSizeInChunks.load(); }
  void setPlanetSizeInChunks(int size) { m_planetSizeInChunks.store(size); }

  int blockToPlace() const { return m_blockToPlace.load(); }
  void setBlockToPlace(int blockId) { m_blockToPlace.store(blockId); }
private:
  // Add any settings you want to store here, e.g.:
  std::atomic<int> m_renderDistance = 8;
  std::atomic<bool> m_vsync = true;
  std::atomic<int> m_warpMode = 0; // 0 = flat, 1 = plane to sphere mapping, 2 = only move y down based on curvature
  std::atomic<bool> m_useTextures = true;
  std::atomic<float> m_textureFadeDistance = 256.0f; // Distance at which textures start fading
  std::atomic<float> m_textureFadeStrength = 1.5f;   // How quickly textures fade out (higher = faster fade)

  std::atomic<float> m_playerSpeed = 20.0f; // units per second

  // World generation settings - ONLY SET BEFORE ENGINE CREATION
  std::atomic<int> m_worldSeed = 0;
  std::atomic<int> m_planetSizeInChunks = 1024; // amount of chunks on one axis

  // For now
  std::atomic<int> m_blockToPlace = 1;
private:
  Settings() = default;
  ~Settings() = default;
  Settings(const Settings &) = delete;
  Settings &operator=(const Settings &) = delete;
};

#endif // SETTINGS_H