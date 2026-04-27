#ifndef CHUNKVERTICES_H
#define CHUNKVERTICES_H

#include <vector>
#include <shared_mutex>
#include <mutex>
#include <atomic>

class ChunkVertices
{
public:
  ChunkVertices()
  {
    m_version.store(0);
    m_calculatingVersion.store(0);
  }

  void setVertices(const std::vector<uint64_t> newVertices[6], uint32_t version)
  {
    uint32_t totalSize = 0;
    for (int i = 0; i < 6; ++i)
    {
      totalSize += newVertices[i].size();
      m_faceVertexCounts[i].store(newVertices[i].size());
    }

    std::vector<uint64_t> newCombinedVertices;
    newCombinedVertices.reserve(totalSize);
    for (int i = 0; i < 6; ++i)
    {
      newCombinedVertices.insert(newCombinedVertices.end(), newVertices[i].begin(), newVertices[i].end());
    }

    std::unique_lock lock(m_verticesMutex);
    uint32_t currentVersion = m_version.load();
    if (version > currentVersion || (currentVersion == UINT32_MAX && currentVersion != version))
    {
      if(!(totalSize == 0 && m_vertices.empty())) {
        m_vertices.swap(newCombinedVertices);
        m_version.store(version);
      }
    }
  }

  void lockShared() { m_verticesMutex.lock_shared(); }
  void unlockShared() { m_verticesMutex.unlock_shared(); }
  const std::vector<uint64_t> &vertices() const { return m_vertices; }

  uint32_t version() const { return m_version.load(); }
  uint32_t calculatingVersion() const { return m_calculatingVersion.load(); }
  uint32_t incrementCalculatingVersion() { return m_calculatingVersion.fetch_add(1) + 1; }

private:
  std::shared_mutex m_verticesMutex;
  std::vector<uint64_t> m_vertices;

  std::atomic<uint32_t> m_faceVertexCounts[6]; // Number of vertices for each face direction
  std::atomic<uint32_t> m_calculatingVersion;
  std::atomic<uint32_t> m_version; // Incremented every time vertices are updated
};

#endif // CHUNKVERTICES_H