#ifndef GLQUICKITEM_H
#define GLQUICKITEM_H

#include "glrenderer.h"

#include <QQuickItem>
#include <QtCore/qtmetamacros.h>
#include <qqmlintegration.h>

class GLQuickItem : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(GLRenderer *renderer READ renderer CONSTANT)
public:
  GLQuickItem();

  GLRenderer *renderer() const { return m_renderer; }

public slots:
  void sync();
  void cleanup();

private slots:
  void handleWindowChanged(QQuickWindow *win);

private:
  void releaseResources() override;

  GLRenderer *m_renderer = nullptr;
};

#endif // GLQUICKITEM_H