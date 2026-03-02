#ifndef OBJECTENGINE_H
#define OBJECTENGINE_H

#include <QObject>

class ObjectEngine : public QObject {
  Q_OBJECT
public:
  ObjectEngine(QObject *parent = nullptr);
};

#endif // OBJECTENGINE_H