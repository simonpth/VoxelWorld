#include "glrenderer.h"
#include "chunkmesh.h"
#include "enginecontext.h"
#include <QtCore/qlogging.h>
#include <QtGui/qopenglfunctions.h>

GLRenderer::GLRenderer() {}

GLRenderer::~GLRenderer() {
  auto it = m_chunkMeshes.begin();
  while (it != m_chunkMeshes.end()) {
    ChunkMesh::cleanupAsync(std::move(it->second));
    it = m_chunkMeshes.erase(it);
  }
  delete m_program;
}

void GLRenderer::init() {
  if (!m_program) {
    QSGRendererInterface *rif = m_window->rendererInterface();
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();
    m_program->addCacheableShaderFromSourceFile(
        QOpenGLShader::Vertex, ":/qt/qml/VoxelWorld/shaders/shader.vert");
    m_program->addCacheableShaderFromSourceFile(
        QOpenGLShader::Fragment, ":/qt/qml/VoxelWorld/shaders/shader.frag");

    m_program->link();

    // for debuging
    const GLubyte *gl_version = glGetString(GL_VERSION);
    const GLubyte *gl_renderer = glGetString(GL_RENDERER);

    qDebug() << "GL_VERSION:" << reinterpret_cast<const char *>(gl_version);
    qDebug() << "GL_RENDERER:" << reinterpret_cast<const char *>(gl_renderer);
    printf("\nGL_VERSION: %s\nGL_RENDERER: %s\n", gl_version, gl_renderer);
  }
}

void GLRenderer::paint() {
  // player movement and logic update
  auto now = std::chrono::steady_clock::now();
  if (m_lastFrame.time_since_epoch().count() != 0) {
    auto delta =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastFrame);

    m_fps = (delta.count() > 0 ? static_cast<int>(1e9 / delta.count()) : 0);
  }
  m_lastFrame = now;

  std::shared_ptr<Engine> engine = EngineContext::instance().engine();

  engine->playerController()->update();

  QMatrix4x4 mvp;
  mvp.perspective(60.0f,
                  m_viewportSize.width() / (float)m_viewportSize.height(), 0.1f,
                  10000.0f);
  mvp.rotate(engine->playerController()->rotation().x(), 1, 0, 0);
  mvp.rotate(engine->playerController()->rotation().y(), 0, 1, 0);
  mvp.translate(-engine->playerController()->position());

  // chunk updates
  auto playerChunk = engine->playerController()->currentChunk();

  if (engine->playerController()->chunksToRenderDirty()) {
    engine->playerController()->setChunksToRenderDirty(false);
    // reset the dirty flag first so that we don't miss any updates
    // that happen during this function
    auto relativeOffsets = engine->playerController()->relativeChunkOffsets();

    std::unordered_map<ChunkPosition, bool> chunksToKeep;
    for (const auto &offset : relativeOffsets) {
      for (int y = 0; y < World::CHUNKHEIGHT; ++y) {
        auto chunkPos = ChunkPosition(playerChunk.x + offset.x, y,
                                      playerChunk.z + offset.z);

        // if the chunk isn't already in the map, create a new mesh for it
        if (m_chunkMeshes.find(chunkPos) == m_chunkMeshes.end()) {
          auto mesh = std::make_unique<ChunkMesh>();
          mesh->setup();
          mesh->setChunkPosition(chunkPos);
          mesh->updateMeshAsync(engine->world());
          m_chunkMeshes.emplace(chunkPos, std::move(mesh));
        }

        chunksToKeep[chunkPos] = true;
      }
    }

    // remove chunks that are no longer in the render distance
    for (auto it = m_chunkMeshes.begin(); it != m_chunkMeshes.end();) {
      if (chunksToKeep.find(it->first) == chunksToKeep.end()) {
        ChunkMesh::cleanupAsync(std::move(it->second));
        it = m_chunkMeshes.erase(it);
      } else {
        ++it;
      }
    }
  }

  // painting
  m_window->beginExternalCommands();

  // SETUP
  // light blue background
  glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  m_program->bind();

  glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // DRAW
  m_program->setUniformValue("mvp_matrix", mvp);

  for (auto &[pos, mesh] : m_chunkMeshes) {
    m_program->setUniformValue("relativeChunkPos",
                               (pos.x - playerChunk.x) * Chunk::SIZE,
                               (pos.y - playerChunk.y) * Chunk::SIZE,
                               (pos.z - playerChunk.z) * Chunk::SIZE);
    mesh->render();
  }

  // CLEANUP FOR QML
  glDisable(GL_DEPTH_TEST);

  m_program->release();

  m_window->endExternalCommands();
  m_window->update();
}
