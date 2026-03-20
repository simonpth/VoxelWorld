#ifndef OBJECT_H
#define OBJECT_H

#include <QVector3D>

class Object {
public:
  Object();

  const QVector3D &position() const{
    return m_position;
  }
  void setPosition(const QVector3D &position){
    m_position = position;
  }
  const QVector3D &rotation() const{
    return m_rotation;
  }
  void setRotation(const QVector3D &rotation){
    m_rotation = rotation;
  }

  virtual void tick();

protected:
  QVector3D m_position;
  QVector3D m_rotation;
};

#endif // OBJECT_H