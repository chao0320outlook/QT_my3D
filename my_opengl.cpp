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

    m_camera.setTranslation(0.0f,0.0f,32.0f);
    m_camera.setRotation(0.0f,1.0f,0.0f,0.0f);
}

My_Opengl::~My_Opengl()
{
    teardownGL();
}

//载入支撑模型
void My_Opengl::load_sample_models(Model model)
{
    Transform3D trans;
    trans.setScale(0.05f,0.05f,1.0f);//基础变形，将模型变换至合适大小
    trans.setTranslation_mid(-model.samples_mid_x(),-model.samples_mid_y(),-model.samples_mid_z()); //将模型移动到正中心变换
    trans.rotate(90.0,1,0,0);
//    sample_move_to_bottom=model.samples_min_y();
//    trans.setTranslation(0.0f,sample_move_to_bottom,0.0f); //使模型置于底部
    for(auto &i :model.vertexs_of_model())
    {
        i.setPosition(My_Vector3D(trans.toMatrix()*i.position()));
    }
    model.sample_restart_size();
    sample_models.push_back(model);
}

//将需支撑的三角面片整合为连续的区域
void My_Opengl::calculate_supperts()
{
    //可优化，直接对需要支撑的区域进行遍历
    if(!meshs_around.isEmpty())
    {
        QVector<bool> mesh_is_counted(models[0].size(),true); //保存面片是否统计
        for(int i=0;i!=models[0].size();++i)
        {
            //若未进行统计且需要支撑
            if(normal_bool[i] && mesh_is_counted[i])
            {
                QVector<int> vec_supperts;     //一个需要支撑的区域
                vec_supperts.push_back(i);
                mesh_is_counted[i]=false;

                count_supperts(i,vec_supperts,mesh_is_counted);
                if(vec_supperts.size()>1)
                    need_supperts_aeras.push_back(vec_supperts);
            }
        }
    }
    emit send_supperts_num(need_supperts_aeras.size());

}
//合成支撑区域 递归子程序
void My_Opengl::count_supperts(int i,QVector<int> &vec_supperts ,QVector<bool> &mesh_is_counted)
{
    //可优化
    for(auto num:meshs_around[i])
    {
        //若未进行统计且需要支撑
        if(mesh_is_counted[num] && normal_bool[num])
        {
            vec_supperts.push_back(num);
            mesh_is_counted[num]=false;
            count_supperts(num,vec_supperts,mesh_is_counted);
        }
    }

}

//将区域投影到x_y轴，并寻找支撑点
void My_Opengl::find_supperts_point()
{
    auto & vertexs=models[0].vertexs_of_model();
    for(auto &ares:need_supperts_aeras)
    {
        float x_min=9999.0f,x_max=-9999.0f,z_min=9999.0f,z_max=-9999.0f;
        //寻找边界
        for(auto i:ares)
        {
            for(int j=0;j!=3;++j)
            {
                if(x_max<vertexs[3*i+j].position().x())
                    x_max=vertexs[3*i+j].position().x();
                else if(x_min>vertexs[3*i+j].position().x())
                    x_min=vertexs[3*i+j].position().x();

                if(z_max<vertexs[3*i+j].position().z())
                    z_max=vertexs[3*i+j].position().z();
                else if(z_min>vertexs[3*i+j].position().z())
                    z_min=vertexs[3*i+j].position().z();
            }
        }
        // 确定支撑位置 依据支撑间隔确定支撑数目
        int x_size=(x_max-x_min)/5+1;
        int z_size=(z_max-z_min)/5+1;
        float x_mid,z_mid;
        if(x_size<2)
            x_size=2;
        x_mid=(x_max-x_min)/x_size;

        if(z_size<2)
            z_size=2;
        z_mid=(z_max-z_min)/x_size;

        //遍历该区域所有待定支撑点
        for(int i=1;i!=x_size;++i)
        {
            for(int j=1;j!=z_size;++j)
            {
                float x=x_min+x_mid*i;
                float y=z_min+z_mid*j;
                check_supperts(x,y,ares);
            }
        }
    }
}
//对网格划分得到的支撑进行mesh匹配，并计算高度
void My_Opengl::check_supperts(const float &x,const float &y,QVector<int>&ares)
{
    auto & vertexs=models[0].vertexs_of_model();
    //向量法判断 点是否在三角形内部
    for(auto i:ares)
    {
        auto x1=vertexs[3*i].position().x();
        auto y1=vertexs[3*i].position().z();

        auto x2=vertexs[3*i+1].position().x();
        auto y2=vertexs[3*i+1].position().z();

        auto x3=vertexs[3*i+2].position().x();
        auto y3=vertexs[3*i+2].position().z();


        //不在，继续循环
        float d=(y-y1)*(x2-x1)-(y2-y1)*(x-x1);
        float q=(y3-y1)*(x2-x1)-(y2-y1)*(x3-x1);
        if(d*q>=0)
        {
            d=(y-y2)*(x3-x2)-(y3-y2)*(x-x2);
            q=(y1-y2)*(x3-x2)-(y3-y2)*(x1-x2);
            if(d*q>=0)
            {
                d=(y-y3)*(x1-x3)-(y1-y3)*(x-x3);
                q=(y2-y3)*(x1-x3)-(y1-y3)*(x2-x3);
                if(d*q>=0)
                {
                    //在该三角形内部
                    X_Y_supperts x_y_position(QVector2D(x,y));
                    x_y_position.set_mesh_num(i);

                    //寻找面片中最低点
                    int i_min=3*i;                               //mesh中y最小的点
                    int y_min=vertexs[3*i].position().y();       //最小点的h
                    if(y_min>vertexs[3*i+1].position().y())
                    {
                        y_min=vertexs[3*i+1].position().y();
                        i_min+=1;
                    }
                    else if(y_min>vertexs[3*i+2].position().y())
                    {
                        y_min=vertexs[3*i+2].position().y();
                        i_min+=2;
                    }
                    x_y_position.set_height(y_min);
                    x_y_ares.push_back(x_y_position);

                    break;
                }
            }
        }
    }
}

//支撑整合为模型
void My_Opengl::set_samples_trans()
{
    calculate_supperts();      //将需支撑的三角面片整合为连续的区域

    find_supperts_point();      //将区域投影到x_y轴，并寻找支撑点

    QVector<My_Vector3D> sample_trans_x_z={{6,0,6},{6,0,-6},{-6,0,6},{-6,0,-6}};

    for(auto& supperts_position:x_y_ares)
    {
        //变换支撑x_z坐标，移动至正确位置
        Transform3D trans;
        float x_move=supperts_position.x()/10.0-models[0].mid_x();
        float z_move=supperts_position.z()/10.0-models[0].mid_z();
        x_move=x_move*zoom_now;
        z_move=z_move*zoom_now;
        trans.setTranslation(x_move,0,z_move);

        //设置支撑高度
        float y_change=supperts_position.y_salce()/sample_models[0].size_Y();   //缩放倍数
        trans.scale(1.0,y_change,1.0);
        trans.translate(0.0,sample_models[0].samples_min_y(y_change),0.0);

        samples_trans.push_back(trans);
    }
    for(int i=0;i!=4;++i)
    {
        Transform3D trans;
        trans.setTranslation(sample_trans_x_z[i]);
        samples_trans.push_back(trans);
    }

    //合成为一整个模型
    for(int i=0;i!=samples_trans.size();++i)
    {
        for(auto &j :sample_models[0].vertexs_of_model())
        {
            //得到变换后位置坐标
           My_Vector3D location = samples_trans[i].toMatrix()*j.position();

           //得到变换后法向量
           QMatrix3x3 normal_c = samples_trans[i].toMatrix().normalMatrix();
           My_Vector3D vector_mine=QMatrix3x3_model(j.normal(),normal_c);
           //添加进新模型
           supports.push_back( Vertex(location,vector_mine));
        }
    }

}

//初始属性设置
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
    if(!VAO_sample.isCreated())
        VAO_sample.create();       //创建VAO
    VBO_sample.create();             //创建VBO
    VAO_sample.bind();             //绑定VAO
    VBO_sample.bind();               //绑定VBO

    my_shader_model->bind();
    my_shader_model->enableAttributeArray(0);
    my_shader_model->enableAttributeArray(1);
    my_shader_model->enableAttributeArray(2);
    my_shader_model->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    my_shader_model->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
    my_shader_model->setAttributeBuffer(2, GL_FLOAT, Vertex::normalOffset(), Vertex::NormalTupleSize, Vertex::stride());

    VBO_sample.setUsagePattern(QOpenGLBuffer::StaticDraw);

    VBO_sample.allocate(begin(supports.vertexs_of_model()),supports.size_vertex()*sizeof(Vertex));
    VBO_sample.release();           //解绑VBO
    VAO_sample.release();         //解绑VAO
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

        VBO_model.allocate(begin(models[0].vertexs_of_model()),models[0].size_vertex()*sizeof(Vertex));
        VBO_model.release();           //解绑VBO
        VAO_model.release();         //解绑VAO
    }
    my_shader_model->release();       //解绑着色器
}

void My_Opengl::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.9, 0.9, 0.9, 1.0);

    glEnable(GL_MULTISAMPLE);  //开启抗锯齿
    glHint(GL_SAMPLES,4);      //设置锯齿绘制点数

    shader_load();
    initializeGL_kaung();
}

//绘制
void My_Opengl::paintGL()
{
    //以下两个if添加的原因是为了initializeGL程序只运行一次
    if(flag_2)
    {
        initializeGL_model();
        flag_2=false;
    }
    if(draw_suppports)
    {
        set_samples_trans();           //生成支撑并整合为一个模型
        initializeGL_sample();         //支撑模型的vao vbo着色器设置
        draw_suppports=false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //绘制框
    Draw_kuang();

    //绘制STL模型
    if(model_readied)
    {
        if(!normal_bool.empty() && show_red!=show_red_pre)
        {
            change_color();
            initializeGL_model();
        }
        Draw_model();
    }

    //绘制支撑
    if(supports.size_vertex())
        Draw_samples();

}
void My_Opengl::Draw_kuang()
{

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
}
void My_Opengl::Draw_samples()
{
    my_shader_model->bind();
    my_shader_model->setUniformValue(my_view,m_camera.toMatrix());        //设置观察变换矩阵
    my_shader_model->setUniformValue(my_projection,m_projection);         //设置投影变换矩阵

    VAO_sample.bind();

    my_shader_model->setUniformValue(my_viepos,m_camera.toMatrix());
    my_shader_model->setUniformValue(my_model, supports_trans.toMatrix());//设置模型变换矩阵
    my_shader_model->setUniformValue(my_normal_model,supports_trans.toMatrix().normalMatrix()); //法向量变换矩阵

    glDrawArrays(GL_TRIANGLES, 0, supports.size_vertex());

    VAO_sample.release();
    my_shader_model->release();
}
void My_Opengl::Draw_model()
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
    QVector<Vertex>& my_normal=models[0].vertexs_of_model();   //得到mesh数组
    QMatrix3x3 normal_c=normal_change.toMatrix().normalMatrix();
    for(auto ptr=my_normal.cbegin();ptr!=my_normal.cend();ptr+=3)
    {
        auto vector_mine=QMatrix3x3_model(ptr->normal(),normal_c);  //得到变换后法向量
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
        initializeOpenGLFunctions();
        initializeGL_model();
    }
}
My_Vector3D My_Opengl::QMatrix3x3_model(My_Vector3D vec,QMatrix3x3 & matrix )
{
    return My_Vector3D
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
        supports_trans.rotate(x_move, up_now);

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
            supports_trans.rotate(y_move, Camera3D::LocalRight);

            emit new_camera(y_move, Camera3D::LocalRight);  //更新主界面的变换矩阵
        }
        emit new_camera(x_move, up_now);
    }

    static const float transSpeed = 0.1f;

    My_Vector3D translation;
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
    My_Vector3D translation;
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

void My_Opengl::camera_restart(float zoom_now)
{
    m_transform[0].rotate(-camera_y, Camera3D::LocalRight);
    m_transform[0].rotate(-camera_x, Camera3D::LocalUp);

    m_transform[1].rotate(-camera_y, Camera3D::LocalRight);
    m_transform[1].rotate(-camera_x, Camera3D::LocalUp);

    camera_x=0.0;
    camera_y=0.0;

    float y= models[0].size_Y()/(20.0/zoom_now)-10.0;

    m_camera.setTranslation(0.0f,y,32.0f);
    m_camera.setRotation(0.0f,1.0f,0.0f,0.0f);
}
