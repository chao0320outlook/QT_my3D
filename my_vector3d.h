#ifndef MY_VECTOR3D_H
#define MY_VECTOR3D_H

#include <QVector3D>
#include <QHash>

class My_Vector3D :public QVector3D
{
public:
    My_Vector3D() =default;

    Q_DECL_CONSTEXPR My_Vector3D(float xpos, float ypos, float zpos) :QVector3D(xpos,ypos,zpos)  {}
    Q_DECL_CONSTEXPR My_Vector3D(QVector3D z) :QVector3D(z)  {}

  friend inline bool operator<(const My_Vector3D &v1, const My_Vector3D &v2)
    {
        if(v1.y()<v2.y())
            return true;
        else if(v1.y()>v2.y())
            return false;
        else
        {
            if(v1.x()<v2.x())
                return true;
            else if(v1.x()>v2.x())
                return false;
            else
            {
                if(v1.z()<v2.z())
                    return true;
                else
                    return false;
            }
        }
    }
  friend inline bool operator>(const My_Vector3D &v1, const My_Vector3D &v2)
    {
        return v1<v2? false:true;
    }
  friend inline bool operator==(const My_Vector3D &v1, const My_Vector3D &v2)
    {
        if(v1.x()==v2.x() && v1.y()==v2.y() && v1.z()==v2.z())
            return true;
        else
            return false;
    }
  friend inline bool operator!=(const My_Vector3D &v1, const My_Vector3D &v2)
    {
        if(v1.x()!=v2.x() || v1.y()!=v2.y() || v1.z()!=v2.z())
            return true;
        else
            return false;
    }

};


#endif // MY_VECTOR3D_H

