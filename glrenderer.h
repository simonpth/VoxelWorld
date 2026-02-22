#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickWindow>

class GLRenderer : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  ~GLRenderer();

  void setViewportSize(const QSize &size) { m_viewportSize = size; }
  void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
  void init();
  void paint();

private:
  QSize m_viewportSize;
  QQuickWindow *m_window = nullptr;

  QOpenGLShaderProgram *m_program = nullptr;
  QOpenGLBuffer m_vbo;
};

#endif // GLRENDERER_H