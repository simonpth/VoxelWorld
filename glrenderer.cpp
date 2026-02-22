#include "glrenderer.h"
#include <OpenGL/gl.h>
#include <QtCore/qlogging.h>

GLRenderer::~GLRenderer() { delete m_program; }

void GLRenderer::init() {
  if (!m_program) {
    QSGRendererInterface *rif = m_window->rendererInterface();
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

    initializeOpenGLFunctions();

    const float values[] = {-1, -1, 1, -1, -1, 1, 1, 1};

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(values, sizeof(values));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                          nullptr);

    m_program = new QOpenGLShaderProgram();
    m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,
                                                ":/qt/qml/VoxelWorld/shaders/shader.vert");
    m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
                                                ":/qt/qml/VoxelWorld/shaders/shader.frag");

    m_program->bindAttributeLocation("vertices", 0);
    m_program->link();
  }
}

void GLRenderer::paint() {
  // Play nice with the RHI. Not strictly needed when the scenegraph uses
  // OpenGL directly.
  m_window->beginExternalCommands();

  m_vbo.bind();
  m_program->bind();
  m_program->setUniformValue("t", 0.5f);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

  glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

  glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableVertexAttribArray(0);
  m_program->release();

  m_window->endExternalCommands();

  m_window->update();
}