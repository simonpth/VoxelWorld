#ifndef GLRENDERER_H
#define GLRENDERER_H

#define GL_SILENCE_DEPRECATION

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickWindow>
#include <QVector3D>

#include <QtCore/qtmetamacros.h>
#include <QtGui/qvectornd.h>
#include <qqmlintegration.h>

class GLRenderer : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
  QML_ELEMENT

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
};

#endif // GLRENDERER_H