#include "transform_3d.h"

//#include<QDebug>
// Transform By (Add/Scale)

void Transform3D::translate(const My_Vector3D &dt)
{
  m_dirty = true;
  m_translation += dt;
}

void Transform3D::scale(const My_Vector3D &ds)
{
  m_dirty = true;
  m_scale *= ds;
}

void Transform3D::rotate(const QQuaternion &dr)
{
  m_dirty = true;
  m_rotation = dr * m_rotation;
}

void Transform3D::grow(const My_Vector3D &ds)
{
  m_dirty = true;
  m_scale += ds;
}

// Transform To (Setters)
void Transform3D::setTranslation(const My_Vector3D &t)
{
  m_dirty = true;
  m_translation = t;
}

void Transform3D::setScale(const My_Vector3D &s)
{
  m_dirty = true;
  m_scale = s;
}

void Transform3D::setRotation(const QQuaternion &r)
{
  m_dirty = true;
  m_rotation = r;
}

const QMatrix4x4 &Transform3D::toMatrix()
{
  if (m_dirty)
  {
    m_dirty = false;
    m_world.setToIdentity();   //初始化为标准矩阵

    //将模型移动至底部
    m_world.translate(m_translation);

    //以下变换顺序不能变！！！！
    //这三个为一个整体，先缩小，后移动至中心，最后进行旋转变换，从下到上，依次执行
    m_world.rotate(m_rotation);
    m_world.translate(m_translation_mid);
    m_world.scale(m_scale);
  }
  return m_world;
}

// Qt Streams
QDebug operator<<(QDebug dbg, const Transform3D &transform)
{
  dbg << "Transform3D\n{\n";
  dbg << "Position: <" << transform.translation().x() << ", " << transform.translation().y() << ", " << transform.translation().z() << ">\n";
  dbg << "Scale: <" << transform.scale().x() << ", " << transform.scale().y() << ", " << transform.scale().z() << ">\n";
  dbg << "Rotation: <" << transform.rotation().x() << ", " << transform.rotation().y() << ", " << transform.rotation().z() << " | " << transform.rotation().scalar() << ">\n}";
  return dbg;
}

QDataStream &operator<<(QDataStream &out, const Transform3D &transform)
{
  out << transform.m_translation;
  out << transform.m_scale;
  out << transform.m_rotation;
  return out;
}

QDataStream &operator>>(QDataStream &in, Transform3D &transform)
{
  in >> transform.m_translation;
  in >> transform.m_scale;
  in >> transform.m_rotation;
  transform.m_dirty = true;
  return in;
}
