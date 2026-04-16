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

  bool warpedWorld() const { return m_warpedWorld.load(); }
  void setWarpedWorld(bool enabled) { m_warpedWorld.store(enabled); }
private:
  // Add any settings you want to store here, e.g.:
  std::atomic<int> m_renderDistance = 8;
  std::atomic<bool> m_vsync = true;
  std::atomic<bool> m_warpedWorld = true;

  std::atomic<float> m_playerSpeed = 20.0f; // units per second

private:
  Settings() = default;
  ~Settings() = default;
  Settings(const Settings &) = delete;
  Settings &operator=(const Settings &) = delete;
};

#endif // SETTINGS_H