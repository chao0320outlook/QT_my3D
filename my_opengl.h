﻿#ifndef MY_OPENGL_H
#define MY_OPENGL_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

#include <QVector>
#include <QVector3D>
#include <Qmap>
#include <QMatrix4x4>

#include <QKeyEvent>
#include <QEvent>

#include "transform_3d.h"
#include "camera.h"
#include "model.h"
#include "my_sample_model.h"

class My_Opengl :public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
     Q_OBJECT
public:
    My_Opengl(QWidget *parent = 0);
    ~My_Opengl();

    Camera3D m_camera;                    //相机成员

    QVector3D vec1[3]=
    {
        {1.0f,0.0f,0.0f},
        {0.0f,0.0f,1.0f},
        {1.0f,0.0f,1.0f},
    };

    void shader_load();
    void initializeGL_kaung();
    void initializeGL_model();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void teardownGL();

    //矩阵与向量相乘  3维
    QVector3D QMatrix3x3_model(QVector3D  vec,QMatrix3x3 & matrix );

    QMatrix3x3 transfrom_0();

    void Draw_mine();          //绘制模型函数，不显示需要支撑位置
    void update_normal();        //需支撑位置判断
    void change_color();
    void set_show_red(bool flag) {show_red=flag;}

    //设置相机移动
    void set_camera_left_true (){camera_left=true;}
    void set_camera_left_false (){camera_left=false;}
    void set_camera_right_true (){camera_right=true;}
    void set_camera_right_false (){camera_right=false;}

    void camera_restart()
    {
        m_transform[0].rotate(-camera_y, Camera3D::LocalRight);
        m_transform[0].rotate(-camera_x, Camera3D::LocalUp);

        m_transform[1].rotate(-camera_y, Camera3D::LocalRight);
        m_transform[1].rotate(-camera_x, Camera3D::LocalUp);

        camera_x=0.0;
        camera_y=0.0;
    }
    void m_transform_restart()
    {
        m_transform[0].restart();
        m_transform[1].restart();

        camera_x=0.0;
        camera_y=0.0;
    }
    QQuaternion get_rotation(){return m_transform[0].rotation();}

    void normal_trans(float angle, QVector3D axis)
    {
        normal_change.rotate(angle,axis);
    }
    void set_normal_trans(float angle, QVector3D axis)
    {
        normal_change.setRotation(angle,axis);
    }

    //读入模型
    void input_model(Model model)
    {
        if(models.empty())
            models.push_back(model);
        else
        {
            models.pop_back();
            models.push_back(model);
        }
    }

    //读入支撑样本
    void load_sample_models(Model model);

    //设置变换
    void set_transform(Transform3D tran){m_transform[1]=tran;}

signals:

    void new_camera(float move,QVector3D now);

protected slots:
    void update();

private slots:
    void updata_models_vector(bool flag);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    //滚轮事件
    void wheelEvent(QWheelEvent *event);

private:

    bool model_readied=false;
    bool flag_2=false;
    bool show_red=false;                   //是否显示需支撑位置
    bool show_red_pre=false;               //上次存储值

    QVector<bool> normal_bool;


    QVector<Model> sample_models;          //支撑样本
    QVector<Model> models;                 //读入模型数据

    QOpenGLTexture *texture[2];                //纹理


    QOpenGLVertexArrayObject VAO_kuang;          //VAO顶点数组对象
    QOpenGLVertexArrayObject VAO_model;

    QOpenGLBuffer  VBO_kuang;                   //VBO顶点缓冲对象
    QOpenGLBuffer  VBO_model;

    QOpenGLShaderProgram *my_shader_kuang;          //绘制框
    QOpenGLShaderProgram *my_shader_model;          //着色器 绘制模型


    //shader_0 uniform值
    int my_model_0;    //模型矩阵
    int my_view_0;     //观察矩阵
    int my_projection_0;   //投影矩阵

    //shader_1 uniform值
    int my_model;    //模型矩阵
    int my_view;     //观察矩阵
    int my_projection;   //投影矩阵
    int my_normal_model;  //法向量变换矩阵

    int my_viepos;       //相机位置


    //相机移动状态
    bool camera_left=false;
    bool camera_right=false;

    //视角变换
    float camera_x=0.0;
    float camera_y=0.0;

    //模型移动至底部的原始偏移量
    float y_move_to_bottom;

    QMatrix4x4 m_projection;             //投影变换矩阵

    Transform3D m_transform[2];          //变换  0为绘制框  1为模型

    Transform3D normal_change;           //计算是否需要支撑时使用的变换矩阵
};

#endif // MY_OPENGL_H
