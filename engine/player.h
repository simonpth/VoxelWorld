#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include <QtGui/qvectornd.h>

const float SPEED = 5.0f;
const float SPRINT_MULTIPLIER = 2.0f;
const float SNEAK_MULTIPLIER = 0.5f;

struct Keys {
  bool forward = false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool jump = false;
  bool sneak = false;
  bool sprint = false;
};

class Player : public Object {
public:
  Player();

  const QVector3D &facingDirection() const { return m_facingDirection; }
  void setFacingDirection(const QVector3D &direction) {
    m_facingDirection = direction;
  }

  void keyPress(const QString &key);
  void keyRelease(const QString &key);

  void tick() override;
  void interpolate(float ms);

private:
  void move(float ms);

  Keys m_keys;
  QVector3D m_facingDirection;
};

#endif // PLAYER_H