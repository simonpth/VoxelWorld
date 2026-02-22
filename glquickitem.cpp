#include "glquickitem.h"

#include <QRunnable>

GLQuickItem::GLQuickItem() {
  connect(this, &QQuickItem::windowChanged, this, &GLQuickItem::handleWindowChanged);
}

void GLQuickItem::handleWindowChanged(QQuickWindow *win) {
  if (win) {
    connect(win, &QQuickWindow::beforeSynchronizing, this, &GLQuickItem::sync, Qt::DirectConnection);
    connect(win, &QQuickWindow::sceneGraphInvalidated, this, &GLQuickItem::cleanup, Qt::DirectConnection);
    win->setColor(Qt::black);
  }
}

void GLQuickItem::sync() {
  if (!m_renderer) {
    m_renderer = new GLRenderer();
    connect(window(), &QQuickWindow::beforeRendering, m_renderer, &GLRenderer::init, Qt::DirectConnection);
    connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &GLRenderer::paint, Qt::DirectConnection);
    m_renderer->setWindow(window());
  }
  m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
}

void GLQuickItem::cleanup() {
  delete m_renderer;
  m_renderer = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(GLRenderer *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    GLRenderer *m_renderer;
};

void GLQuickItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}