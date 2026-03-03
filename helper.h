#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <qqmlintegration.h>
#include <QGuiApplication>
#include <QScreen>
#include <QCursor>

class Helper : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  Q_INVOKABLE void moveCursorToCenter() {
    if (QGuiApplication::primaryScreen()) {
      QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
      QPoint center = screenGeometry.center();
      QCursor::setPos(center);
    }
  }
};

#endif // HELPER_H
