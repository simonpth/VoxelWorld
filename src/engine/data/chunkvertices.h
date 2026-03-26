#ifndef CHUNKVERTICES_H
#define CHUNKVERTICES_H

#include <vector>
#include <shared_mutex>
#include <atomic>

class ChunkVertices
{
public:
  ChunkVertices() { m_version.store(0); }

  void setVertices(const std::vector<uint64_t> newVertices[6])
  {
    uint32_t totalSize = 0;
    for (int i = 0; i < 6; ++i)
    {
      totalSize += newVertices[i].size();
      m_faceVertexCounts[i].store(newVertices[i].size());
    }

    std::vector <uint64_t> emptyVertices;
    emptyVertices.reserve(totalSize);
    for (int i = 0; i < 6; ++i)
    {
      emptyVertices.insert(emptyVertices.end(), newVertices[i].begin(), newVertices[i].end());
    }

    std::unique_lock lock(m_verticesMutex);
    m_vertices.swap(emptyVertices);
    m_version.fetch_add(1);
  }

  void lockShared() { m_verticesMutex.lock_shared(); }
  void unlockShared() { m_verticesMutex.unlock_shared(); }
  const std::vector<uint64_t> &vertices() const { return m_vertices; }

  uint32_t version() const { return m_version.load(); }

private:
  std::shared_mutex m_verticesMutex;
  std::vector<uint64_t> m_vertices;

  std::atomic<uint32_t> m_faceVertexCounts[6];  // Number of vertices for each face direction
  std::atomic<uint32_t> m_version; // Incremented every time vertices are updated
};

#endif // CHUNKVERTICES_H