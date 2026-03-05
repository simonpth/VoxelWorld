#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QtCore/qpoint.h>
#include <qqmlintegration.h>
#include <QCursor>

class Helper : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  Q_INVOKABLE void moveCursorToScreenCoords(QPoint pos) {
    QCursor::setPos(pos);
  }
};

#endif // HELPER_H
