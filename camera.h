#ifndef CAMERA_H
#define CAMERA_H

#include "transform_3d.h"

class Camera3D
{
public:
  //默认三个方向轴
  static const QVector3D LocalForward;
  static const QVector3D LocalUp;
  static const QVector3D LocalRight;

  Camera3D() : m_dirty(true) {}

  // 位置变化多少  By (Add/Scale)
  void translate(const QVector3D &dt);
  void translate(float dx, float dy, float dz){ translate(QVector3D(dx, dy, dz)); }

  void rotate(const QQuaternion &dr);
  void rotate(float angle, const QVector3D &axis) { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
  void rotate(float angle, float ax, float ay, float az){ rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

  // 位置变化  为   To (Setters)
  void setTranslation(const QVector3D &t);
  void setTranslation(float x, float y, float z){ setTranslation(QVector3D(x, y, z)); }
  void setRotation(const QQuaternion &r);
  void setRotation(float angle, const QVector3D &axis){ setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
  void setRotation(float angle, float ax, float ay, float az){ setRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }


  // 返回变化信息
  const QVector3D& translation() const{ return m_translation; }
  const QQuaternion& rotation() const { return m_rotation; }
  const QMatrix4x4& toMatrix();

  // 返回三个轴
  QVector3D forward() const;
  QVector3D right() const;
  QVector3D up() const;

private:
  bool m_dirty;   //相机是否移动标志
  QVector3D m_translation;  //位移
  QQuaternion m_rotation;   //旋转
  QMatrix4x4 m_world;       //最终变换矩阵

#ifndef QT_NO_DATASTREAM
  friend QDataStream &operator<<(QDataStream &out, const Camera3D &transform);
  friend QDataStream &operator>>(QDataStream &in, Camera3D &transform);
#endif
};

Q_DECLARE_TYPEINFO(Camera3D, Q_MOVABLE_TYPE);

// Qt Streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Camera3D &transform);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const Camera3D &transform);
QDataStream &operator>>(QDataStream &in, Camera3D &transform);
#endif

#endif // CAMERA_H
