#ifndef GLRENDERER_H
#define GLRENDERER_H

#define GL_SILENCE_DEPRECATION

#include "engine/engine.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickWindow>
#include <QVector3D>

#include <QtCore/qtmetamacros.h>
#include <QtGui/qvectornd.h>
#include <qqmlintegration.h>

#include <chrono>

class GLRenderer : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(Engine *engine WRITE setEngine)
public:
  ~GLRenderer();

  void setViewportSize(const QSize &size) { m_viewportSize = size; }
  void setWindow(QQuickWindow *window) { m_window = window; }

  void setEngine(Engine *engine) { m_engine = engine; }

public slots:
  void init();
  void paint();

private:
  Engine *m_engine = nullptr;

  QSize m_viewportSize;
  QQuickWindow *m_window = nullptr;

  QOpenGLShaderProgram *m_program = nullptr;

  QOpenGLBuffer m_vbo;
  QOpenGLVertexArrayObject m_vao;

  std::chrono::high_resolution_clock::time_point m_lastFrame;
};

#endif // GLRENDERER_H