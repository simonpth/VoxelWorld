#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QVector3D>
#include <QtGui/qvectornd.h>
#include <qqmlintegration.h>
#include <QMutex>

struct PlayerControllerInput {
  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool jump = false;
};

class PlayerController : public QObject {
  Q_OBJECT
  QML_ELEMENT
public:
  PlayerController(QObject *parent = nullptr);

  Q_INVOKABLE void keyPressed(const int &key);
  Q_INVOKABLE void keyReleased(const int &key);

  Q_INVOKABLE void mouseMoved(const float &deltaX, const float &deltaY);

private:
  QVector3D m_position;
  QVector3D m_velocity;
  QVector3D m_rotation; // pitch, yaw, roll

  PlayerControllerInput m_input;
  QMutex m_inputMutex;
};

#endif // PLAYERCONTROLLER_H