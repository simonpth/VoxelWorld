#define GL_SILENCE_DEPRECATION

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "engine/enginecontext.h"

int useQmlGLRenderer(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setSwapInterval(QSurfaceFormat::DefaultSwapBehavior);
  QSurfaceFormat::setDefaultFormat(format);

  QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

  EngineContext::instance().createEngine();

  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("VoxelWorld", "Main");

  int result = app.exec();

  EngineContext::instance().deleteEngine();

  return result;
}

int main(int argc, char *argv[]) {
  return useQmlGLRenderer(argc, argv);
}
