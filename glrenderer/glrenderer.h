#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <QtCore/qtmetamacros.h>
#define GL_SILENCE_DEPRECATION

#include "engine/engine.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QQuickWindow>
#include <QVector3D>
#include <qqmlintegration.h>

#include <chrono>

class GLRenderer : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(Engine *engine WRITE setEngine)
  Q_PROPERTY(int fps READ fps)
public:
  GLRenderer();
  ~GLRenderer();

  void setViewportSize(const QSize &size) { m_viewportSize = size; }
  void setWindow(QQuickWindow *window) { m_window = window; }

  void setEngine(Engine *engine) { m_engine = engine; }

  int fps() const { return m_fps; }

public slots:
  void init();
  void paint();

private:
  Engine *m_engine = nullptr;
  int m_fps = 0;

  QSize m_viewportSize;
  QQuickWindow *m_window = nullptr;

  QOpenGLShaderProgram *m_program = nullptr;

  QOpenGLBuffer m_vbo;
  QOpenGLVertexArrayObject m_vao;

  std::chrono::steady_clock::time_point m_lastFrame;
};

#endif // GLRENDERER_H