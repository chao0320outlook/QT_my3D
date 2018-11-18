#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include "mesh.h"

class Model
{
public:
    Model()=default;

    void push_back(const Mesh & mesh_s) {meshs.push_back(mesh_s);}
    void push_back(Mesh && mesh_s) noexcept {meshs.push_back(std::move(mesh_s));}

    void push_back(const QVector3D & vertex) {vertexs.push_back(vertex);}
    void push_back(QVector3D && vertex) noexcept {vertexs.push_back(std::move(vertex));}

    void push_back(const Vertex & vertex) {vertexs_2.push_back(vertex);}
    void push_back(Vertex && vertex) noexcept {vertexs_2.push_back(std::move(vertex));}

    int size()const {return meshs.size();}   //返回mesh数目
    int size_vertex()const {return vertexs.size();}  //返回顶点数目

    void set_size(QVector <float> vec)    //设置模型的三轴最大最小值以及中间点
    {
        x_max=vec[0];
        x_min=vec[1];
        y_max=vec[2];
        y_min=vec[3];
        z_max=vec[4];
        z_min=vec[5];

        y_mid=y_min+(y_max-y_min)/2.0;
        x_mid=x_min+(x_max-x_min)/2.0;
        z_mid=z_min+(z_max-z_min)/2.0;
    }

    float size_X()const{return x_max-x_min;}  //返回X轴上长度
    float size_Y()const{return y_max-y_min;}  //返回Y轴上长度
    float size_Z()const{return z_max-z_min;}  //返回Z轴上长度

    float min_y(float num)const //获得将模型置于底部的偏移量
    {
        if(num!=1)
            return (y_max-y_mid)/(10.0/num)-9.5f;   //注意！！！！！！！！！是乘除的关系ss
        else
            return (y_max-y_mid)/10.0f-9.5f;
    }
    float mid_y()const {return y_mid/10.0;}  //获得y中心坐标
    float mid_x()const {return x_mid/10.0;}  //获得x中心坐标
    float mid_z()const {return z_mid/10.0;}   //获得z中心坐标

    void set_color(int i)
    {
        vertexs_2[i].setColor(1.0,0.0,0.0);
        vertexs_2[i+1].setColor(1.0,0.0,0.0);
        vertexs_2[i+2].setColor(1.0,0.0,0.0);
    }
    void set_color_2(int i)
    {
        vertexs_2[i].setColor(0.0,1.0,0.0);
        vertexs_2[i+1].setColor(0.0,1.0,0.0);
        vertexs_2[i+2].setColor(0.0,1.0,0.0);
    }
    void restart_color()
    {
        for(auto & i:vertexs_2)
            i.setColor(0.3,0.3,0.3);
    }

    QVector<Mesh>& meshs_of_model(){return meshs;}
    QVector<QVector3D>& vertex_of_model(){return vertexs;}
    QVector<Vertex>& vertex_2_of_model(){return vertexs_2;}

    void clear()
    {
        meshs.clear();
        vertexs.clear();
        vertexs_2.clear();
    }

private:
    QVector<Mesh> meshs;          //mesh数组
    QVector<QVector3D> vertexs;   //顶点数组，只包含坐标
    QVector<Vertex> vertexs_2;    //顶点数组，包含坐标与法向量 绘制时使用
    float x_max,x_min,y_max,y_min,z_max,z_min; //三轴实际最大最小值
    float x_mid,y_mid,z_mid;      //中心位置坐标
};

#endif // MODEL_H
