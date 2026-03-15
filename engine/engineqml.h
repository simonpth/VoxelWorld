#ifndef ENGINEQML_H
#define ENGINEQML_H

#include "enginecontext.h"

#include <QtCore/qobjectdefs.h>
#include <QtCore/qtmetamacros.h>
#include <qqmlintegration.h>

class EngineQml : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(EngineQML)
  QML_SINGLETON
public:
  explicit EngineQml(QObject *parent = nullptr) : QObject(parent) {}
  Q_INVOKABLE Engine *instance() {
    return EngineContext::instance().engine().get();
  }
};

#endif // ENGINEQML_H