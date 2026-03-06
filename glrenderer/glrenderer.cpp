#include "glrenderer.h"
#include <OpenGL/gl.h>
#include <QtCore/qlogging.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopengl.h>
#include <chrono>

GLRenderer::GLRenderer() {}

GLRenderer::~GLRenderer() { delete m_program; }

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

    // setup cube
    float vertices[] = {
        // front face (z = 1)
        0.0f, 0.0f, 1.0f, // bottom-left
        1.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, 1.0f, // top-right
        1.0f, 1.0f, 1.0f, // top-right
        0.0f, 1.0f, 1.0f, // top-left
        0.0f, 0.0f, 1.0f, // bottom-left

        // back face (z = 0)
        0.0f, 0.0f, 0.0f, // bottom-left
        0.0f, 1.0f, 0.0f, // top-left
        1.0f, 1.0f, 0.0f, // top-right
        1.0f, 1.0f, 0.0f, // top-right
        1.0f, 0.0f, 0.0f, // bottom-right
        0.0f, 0.0f, 0.0f, // bottom-left

        // top face (y = 1)
        0.0f, 1.0f, 0.0f, // back-left
        0.0f, 1.0f, 1.0f, // front-left
        1.0f, 1.0f, 1.0f, // front-right
        1.0f, 1.0f, 1.0f, // front-right
        1.0f, 1.0f, 0.0f, // back-right
        0.0f, 1.0f, 0.0f, // back-left

        // bottom face (y = 0)
        0.0f, 0.0f, 0.0f, // back-left
        1.0f, 0.0f, 0.0f, // back-right
        1.0f, 0.0f, 1.0f, // front-right
        1.0f, 0.0f, 1.0f, // front-right
        0.0f, 0.0f, 1.0f, // front-left
        0.0f, 0.0f, 0.0f, // back-left

        // right face (x = 1)
        1.0f, 0.0f, 0.0f, // bottom-back
        1.0f, 1.0f, 0.0f, // top-back
        1.0f, 1.0f, 1.0f, // top-front
        1.0f, 1.0f, 1.0f, // top-front
        1.0f, 0.0f, 1.0f, // bottom-front
        1.0f, 0.0f, 0.0f, // bottom-back

        // left face (x = 0)
        0.0f, 0.0f, 0.0f, // bottom-back
        0.0f, 0.0f, 1.0f, // bottom-front
        0.0f, 1.0f, 1.0f, // top-front
        0.0f, 1.0f, 1.0f, // top-front
        0.0f, 1.0f, 0.0f, // top-back
        0.0f, 0.0f, 0.0f  // bottom-back
    };

    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

    m_vao.release();

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

    m_engine->playerController()->update(delta);
  }
  m_lastFrame = now;

  QMatrix4x4 mvp;
  mvp.perspective(60.0f,
                  m_viewportSize.width() / (float)m_viewportSize.height(), 0.1f,
                  10000.0f);
  mvp.rotate(m_engine->playerController()->rotation().x(), 1, 0, 0);
  mvp.rotate(m_engine->playerController()->rotation().y(), 0, 1, 0);
  mvp.translate(-m_engine->playerController()->position());

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
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // DRAW
  m_vao.bind();
  m_program->setUniformValue("mvp_matrix", mvp);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  m_vao.release();

  // CLEANUP FOR QML
  glDisable(GL_DEPTH_TEST);

  m_program->release();

  m_window->endExternalCommands();
  m_window->update();
}
