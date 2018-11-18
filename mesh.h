#ifndef MESH_H
#define MESH_H
#include "vertex.h"

class Mesh
{
public:
    Q_DECL_CONSTEXPR Mesh(){}

    Q_DECL_CONSTEXPR explicit Mesh(const QVector3D vecto,const Vertex verte_x
                                   ,const Vertex verte_y,const Vertex verte_z)
        :m_vector(vecto),m_1(verte_x),m_2(verte_y),m_3(verte_z){}
    const QVector3D& get_m_vector()const {return m_vector;}
    Vertex get_m_1() const {return m_1;}
    Vertex get_m_2() const {return m_2;}
    Vertex get_m_3() const {return m_3;}
private:
    QVector3D m_vector;     //法向量
    Vertex m_1;             //顶点1
    Vertex m_2;             //顶点2
    Vertex m_3;             //顶点3
};

#endif // MESH_H
