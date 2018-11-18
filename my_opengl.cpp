#include "my_opengl.h"
#include <QDebug>
#include <QKeyEvent>
#include <QEvent>
#include <QString>
#include <memory>
#include <QOpenGLFunctions_4_5_Core>
#include "vertex.h"
#include "input.h"

using namespace  std;
static const double pi_mine=0.017453;

My_Opengl::My_Opengl(QWidget *parent):QOpenGLWidget(parent)
{
    //设置OpenGL的版本信息
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,5);
    setFormat(format);

    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));  //必须

    m_camera.setTranslation(0.0f,6.0f,32.0f);
    m_camera.setRotation(-12.0f,1.0f,0.0f,0.0f);
}

My_Opengl::~My_Opengl()
{
    teardownGL();
}


void My_Opengl::load_sample_models(Model model)
{
    Transform3D trans;
    trans.setScale(0.1f,0.1f,0.1f);//基础变形，缩小10倍
    trans.setTranslation_mid(-model.mid_x(),-model.mid_y(),-model.mid_z()); //将模型移动到正中心变换

    for(auto &i :model.vertex_2_of_model())
    {
        i.setPosition(trans.toMatrix()*i.position());
    }

    sample_models.push_back(model);
}

void My_Opengl::shader_load()
{
    // 创建着色器
    my_shader_kuang = new QOpenGLShaderProgram();
    my_shader_kuang->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert");
    my_shader_kuang->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag");

    my_shader_kuang->bindAttributeLocation("position", 0);
    my_shader_kuang->bindAttributeLocation("aTexCoords", 1);
    my_shader_kuang->link();

    //uniform变量绑定
    my_shader_kuang->bind();
    my_shader_kuang->setUniformValue("texture", 0);
    my_model_0=my_shader_kuang->uniformLocation("model");
    my_view_0=my_shader_kuang->uniformLocation("view");
    my_projection_0 =my_shader_kuang->uniformLocation("projection");
    my_shader_kuang->release();       //解绑着色器


    // 创建着色器
    my_shader_model = new QOpenGLShaderProgram();
    my_shader_model->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader_model.vert");
    my_shader_model->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader_model.frag");

    my_shader_model->bindAttributeLocation("position", 0);
    my_shader_model->bindAttributeLocation("color", 1);
    my_shader_model->bindAttributeLocation("ver_normal", 2);
    my_shader_model->link();

    //uniform变量绑定
    my_shader_model->bind();
    my_model=my_shader_model->uniformLocation("model");
    my_view=my_shader_model->uniformLocation("view");
    my_projection =my_shader_model->uniformLocation("projection");
    my_normal_model=my_shader_model->uniformLocation("normal_model");
    my_viepos=my_shader_model->uniformLocation("viewpos");
    my_shader_model->release();
}

void My_Opengl::initializeGL_kaung()
{
    //QOpenGLVertexArrayObject不能使用vector存储，原因不明
    my_shader_kuang->release();       //解绑着色器
    if(!VAO_kuang.isCreated())
        VAO_kuang.create();          //创建VAO
    VAO_kuang.bind();            //绑定VAO

    if(!VBO_kuang.isCreated())
        VBO_kuang.create();       //创建VBO
    VBO_kuang.bind();         //绑定VBO
    VBO_kuang.setUsagePattern(QOpenGLBuffer::StaticDraw);
    VBO_kuang.allocate(static_my_box, sizeof(static_my_box));

    my_shader_kuang->enableAttributeArray(0);
    my_shader_kuang->enableAttributeArray(1);
    my_shader_kuang->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    my_shader_kuang->setAttributeBuffer(1, GL_FLOAT, Vertex::textureOffset(), Vertex::TextureTupleSize, Vertex::stride());


    VBO_kuang.release();           //解绑VBO
    VAO_kuang.release();              //解绑VAO
    my_shader_kuang->release();       //解绑着色器

    texture[0]=new QOpenGLTexture(QImage(QString(":images/vector_1")).mirrored());
}

void My_Opengl::initializeGL_sample()
{
    //模型载入缓冲区
    if(model_readied)
    {
        if(!VAO_model.isCreated())
            VAO_model.create();       //创建VAO
        VBO_model.create();             //创建VBO
        VAO_model.bind();             //绑定VAO
        VBO_model.bind();               //绑定VBO

        my_shader_model->bind();
        my_shader_model->enableAttributeArray(0);
        my_shader_model->enableAttributeArray(1);
        my_shader_model->enableAttributeArray(2);
        my_shader_model->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        my_shader_model->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
        my_shader_model->setAttributeBuffer(2, GL_FLOAT, Vertex::normalOffset(), Vertex::NormalTupleSize, Vertex::stride());

        VBO_model.setUsagePattern(QOpenGLBuffer::StaticDraw);

        VBO_model.allocate(begin(models[0].vertex_2_of_model()),models[0].size_vertex()*sizeof(Vertex));
        VBO_model.release();           //解绑VBO
        VAO_model.release();         //解绑VAO
    }
    my_shader_model->release();       //解绑着色器
}

void My_Opengl::initializeGL_model()
{
    //模型载入缓冲区
    if(model_readied)
    {
        if(!VAO_model.isCreated())
            VAO_model.create();       //创建VAO
        VBO_model.create();             //创建VBO
        VAO_model.bind();             //绑定VAO
        VBO_model.bind();               //绑定VBO

        my_shader_model->bind();
        my_shader_model->enableAttributeArray(0);
        my_shader_model->enableAttributeArray(1);
        my_shader_model->enableAttributeArray(2);
        my_shader_model->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        my_shader_model->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
        my_shader_model->setAttributeBuffer(2, GL_FLOAT, Vertex::normalOffset(), Vertex::NormalTupleSize, Vertex::stride());

        VBO_model.setUsagePattern(QOpenGLBuffer::StaticDraw);

        VBO_model.allocate(begin(models[0].vertex_2_of_model()),models[0].size_vertex()*sizeof(Vertex));
        VBO_model.release();           //解绑VBO
        VAO_model.release();         //解绑VAO
    }
    my_shader_model->release();       //解绑着色器
}

void My_Opengl::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.9, 0.9, 0.9, 1.0f);

    glEnable(GL_MULTISAMPLE);  //开启抗锯齿
    glHint(GL_SAMPLES,4);      //设置锯齿绘制点数

    shader_load();

    initializeGL_kaung();
    initializeGL_model();
}

void My_Opengl::paintGL()
{
    if(flag_2)
    {
        initializeGL();
        flag_2=false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    //绘制打印框
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);
    VAO_kuang.bind();

    my_shader_kuang->bind();
    my_shader_kuang->setUniformValue(my_view_0,m_camera.toMatrix());        //设置观察变换矩阵
    my_shader_kuang->setUniformValue(my_projection_0,m_projection);         //设置投影变换矩阵

    m_transform[0].setScale(7.50f,10.0f,7.50f);  //2*2*2的方形框变换为15*15*20 的长方体
    my_shader_kuang->setUniformValue(my_model_0, m_transform[0].toMatrix());     //设置模型变换矩阵

    for(int i=0;i<6;++i)
    {
        texture[0]->bind();
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
    my_shader_kuang->release();
    VAO_kuang.release();

    glEnable(GL_CULL_FACE);

    //绘制STL模型
    if(model_readied)
    {
        if(!normal_bool.empty())
        {
            if(show_red!=show_red_pre)
            {
                change_color();
                initializeGL();
            }
        }
        Draw_mine();
    }
}

//模型绘制
void My_Opengl::Draw_mine()
{
    my_shader_model->bind();
    my_shader_model->setUniformValue(my_view,m_camera.toMatrix());        //设置观察变换矩阵
    my_shader_model->setUniformValue(my_projection,m_projection);         //设置投影变换矩阵

    VAO_model.bind();

    my_shader_model->setUniformValue(my_viepos,m_camera.toMatrix());
    my_shader_model->setUniformValue(my_model, m_transform[1].toMatrix());//设置模型变换矩阵
    my_shader_model->setUniformValue(my_normal_model,m_transform[1].toMatrix().normalMatrix());

    glDrawArrays(GL_TRIANGLES, 0, models[0].size_vertex());

    VAO_model.release();
    my_shader_model->release();
}

//更新normal_bool
void My_Opengl::update_normal()
{
    if(!normal_bool.empty())
        normal_bool.clear();
    auto my_normal=models[0].meshs_of_model();   //得到mesh数组
    QMatrix3x3 normal_c=normal_change.toMatrix().normalMatrix();
    for(auto &i:my_normal)
    {
        auto vector_mine=QMatrix3x3_model(i.get_m_vector(),normal_c);  //得到变换后法向量
        //若y轴法向量为负
        if(vector_mine.y()<0)
        {
            auto x_z=sqrt(vector_mine.x()*vector_mine.x()+vector_mine.z()*vector_mine.z());
            //若角度大于45
            if(-(vector_mine.y()) > x_z)
                normal_bool.push_back(true);
            else
                normal_bool.push_back(false);
        }
        else
            normal_bool.push_back(false);
    }
    if(show_red)
    {
        change_color();
        initializeGL();
    }
}
QVector3D My_Opengl::QMatrix3x3_model(QVector3D vec,QMatrix3x3 & matrix )
{
    return QVector3D
            (vec.x() * matrix(0,0) + vec.y() * matrix(0,1) +vec.z()* matrix(0,2),
             vec.x() * matrix(1,0) + vec.y() * matrix(1,1) +vec.z()* matrix(1,2),
             vec.x() * matrix(2,0) + vec.y() * matrix(2,1) +vec.z()* matrix(2,2));
}

QMatrix3x3  My_Opengl::transfrom_0()
{
    return m_transform[0].toMatrix().normalMatrix();
}

//更改颜色属性
void My_Opengl::change_color()
{
    show_red_pre=show_red;
    if(show_red)
    {

        for(int i=0,j=0;i!=models[0].size_vertex();i=i+3,++j)
        {
            if(normal_bool[j])
                models[0].set_color(i);
            else
                models[0].set_color_2(i);
        }
    }
    else
        models[0].restart_color();
}

void My_Opengl::teardownGL()
{
    VBO_model.destroy();
    VBO_model.destroy();

    VAO_kuang.destroy();
    VAO_model.destroy();

    delete my_shader_kuang;
    delete my_shader_model;
}

void My_Opengl::resizeGL(int width, int height)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, width / float(height), 0.01f, 100.0f);   //！！！！！找了很久的BUG！！！！！
}

void My_Opengl::update()
{
    // 输入更新
    Input::update();
    if (Input::buttonPressed(Qt::RightButton))
    {
        QMatrix3x3 up_change=transfrom_0();
        auto up_now=QMatrix3x3_model(Camera3D::LocalUp,up_change);

        static const float rotSpeed   = 0.3f;
        auto x_move=rotSpeed * Input::mouseDelta().x();
        auto y_move=rotSpeed * Input::mouseDelta().y();

        auto flag=y_move+camera_y;

        camera_x+=x_move;
        m_transform[0].rotate(x_move, up_now);
        m_transform[1].rotate(x_move, up_now);

        if(flag<90.0&&flag>-90.0)
        {
            camera_y+=y_move;

            //观察角度变换后，改变移动至底部中央的translation
            if(model_readied)
            {
                auto y_trans=cos(camera_y * pi_mine)*y_move_to_bottom;
                auto z_trans=sin(camera_y * pi_mine)*y_move_to_bottom;

                m_transform[1].setTranslation(0.0,y_trans,z_trans);
            }

            m_transform[0].rotate(y_move, Camera3D::LocalRight);
            m_transform[1].rotate(y_move, Camera3D::LocalRight);

            emit new_camera(y_move, Camera3D::LocalRight);  //更新主界面的变换矩阵
        }
        emit new_camera(x_move, up_now);
    }

    static const float transSpeed = 0.1f;
    QVector3D translation;

    if (camera_left)
    {
        translation -= m_camera.right();
    }
    if (camera_right)
    {
        translation += m_camera.right();
    }

    m_camera.translate(transSpeed * translation);
    // Schedule a redraw
    QOpenGLWidget::update();
}

//鼠标事件
void My_Opengl::mousePressEvent(QMouseEvent *event)
{
  Input::registerMousePress(event->button());
}
void My_Opengl::mouseReleaseEvent(QMouseEvent *event)
{
  Input::registerMouseRelease(event->button());
}

//滚轮事件
void My_Opengl::wheelEvent(QWheelEvent *event)
{
    static const float transSpeed = 1.0f;
    QVector3D translation;
    if(event->delta()>0)     //若滚轮上滚
        translation += m_camera.forward();
    else
        translation -= m_camera.forward();
    m_camera.translate(transSpeed * translation);
    QOpenGLWidget::update();
}

//接受主程序发来的信号，进行模型环境渲染
void My_Opengl::updata_models_vector(bool flag)
{
    if(flag)
    {
        model_readied=true;  //读入模型数据 设置为true
        y_move_to_bottom=m_transform[1].translation().y();     //读入原始偏移量
        flag_2=true;         //修复渲染bug添加的 布尔变量，读入模型后设置为真，而后调用一次initializeGL()函数，设置为假
    }
}

