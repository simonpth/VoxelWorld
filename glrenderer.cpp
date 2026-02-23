#include "glrenderer.h"
#include <OpenGL/gl.h>
#include <QtCore/qlogging.h>
#include <QtGui/qopengl.h>

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

    // for debuging
    const GLubyte *gl_version = glGetString(GL_VERSION);
    const GLubyte *gl_renderer = glGetString(GL_RENDERER);

    printf("\nGL_VERSION:\n%s\nGL_RENDERER:\n%s\n", gl_version, gl_renderer);
  }
}

void GLRenderer::paint() {
  m_window->beginExternalCommands();

  m_program->bind();

  glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
  glDisable(GL_DEPTH_TEST);

  m_program->release();

  m_window->endExternalCommands();
  m_window->update();
}
