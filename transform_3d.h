﻿#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

//物体三维改变类

#include <QVector3D>
#include <QQuaternion>    //四元数
#include <QMatrix4x4>

class Transform3D
{
public:

  Transform3D():m_dirty(true), m_scale(1.0f, 1.0f, 1.0f) {}

  //位移
  void translate(const QVector3D &dt);
  void translate(float dx, float dy, float dz){ translate(QVector3D(dx, dy, dz)); }

  void translate_mid(float dx, float dy, float dz){m_translation_mid+=QVector3D(dx, dy, dz);}

  //缩放
  void scale(const QVector3D &ds);
  void scale(float dx, float dy, float dz){ scale(QVector3D(dx, dy, dz)); }
  void scale(float factor){ scale(QVector3D(factor, factor, factor)); }

  //旋转
  void rotate(const QQuaternion &dr);
  void rotate(float angle, const QVector3D &axis){ rotate(QQuaternion::fromAxisAndAngle(axis, angle));}
  void rotate(float angle, float ax, float ay, float az){ rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

  //快速缩放
  void grow(const QVector3D &ds);
  void grow(float dx, float dy, float dz){ grow(QVector3D(dx, dy, dz)); }
  void grow(float factor){ grow(QVector3D(factor, factor, factor)); }

  //设置位置矩阵
  void setTranslation(const QVector3D &t);
  void setTranslation(float x, float y, float z){ setTranslation(QVector3D(x, y, z)); }

  void setTranslation_mid(float x, float y, float z) {m_translation_mid=QVector3D(x, y, z);}


  //设置缩放矩阵
  void setScale(const QVector3D &s);
  void setScale(float x, float y, float z){ setScale(QVector3D(x, y, z)); }
  void setScale(float k){ setScale(QVector3D(k, k, k)); }

  //设置旋转矩阵
  void setRotation(const QQuaternion &r);
  void setRotation(float angle, const QVector3D &axis){ setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
  void setRotation(float angle, float ax, float ay, float az){ setRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }


  void restart()
  {
      m_dirty=true;
      m_translation=QVector3D(0,0,0);
      m_translation_mid=QVector3D(0,0,0);
      m_scale=QVector3D(1.0,1.0,1.0);
      m_rotation=QQuaternion(1.0,0.0,0.0,0.0);
  }
  const QVector3D& translation() const{ return m_translation; }
  const QVector3D& scale() const{ return m_scale; }
  const QQuaternion& rotation() const{ return m_rotation; }
  //返回最终改变
  const QMatrix4x4& toMatrix();

private:
  bool m_dirty;                //类内私有成员改变标志位
  QVector3D m_translation;     //位置矩阵
  QVector3D m_translation_mid; //移动到中间点的变换
  QVector3D m_scale;           //缩放矩阵
  QQuaternion m_rotation;      //旋转四元数
  QMatrix4x4 m_world;          //所有改变综合后矩阵

#ifndef QT_NO_DATASTREAM
  friend QDataStream &operator<<(QDataStream &out, const Transform3D &transform);
  friend QDataStream &operator>>(QDataStream &in, Transform3D &transform);
#endif
};

Q_DECLARE_TYPEINFO(Transform3D, Q_MOVABLE_TYPE);

// Qt Streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Transform3D &transform);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const Transform3D &transform);
QDataStream &operator>>(QDataStream &in, Transform3D &transform);

#endif
#endif // TRANSFORM3D_H
