﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include "model.h"

static const char first_stl_binarry=0x53;
static const char first_stl_ASSCII=0x73;

static const My_Vector3D my_x(1.0,0.0,0.0);
static const My_Vector3D my_y(0.0,1.0,0.0);
static const My_Vector3D my_z(0.0,0.0,1.0);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置界面显示
    ui->groupBox->setVisible(false);
    ui->groupBox_2->setVisible(false);

    //将读入模型的信号通知opengl
    connect(this,SIGNAL(new_model_data(bool)),ui->openGLWidget,SLOT(updata_models_vector(bool)));
    connect(ui->openGLWidget,SIGNAL(new_camera(float,My_Vector3D)),this,SLOT(updata_camera_x(float,My_Vector3D)));
    connect(ui->openGLWidget,SIGNAL(send_supperts_num(int)),this,SLOT(show_supperts_num(int)));

    load_sample_models();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//键盘控制
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_A)
        ui->openGLWidget->set_camera_left_true();
    if(event->key()==Qt::Key_D)
        ui->openGLWidget->set_camera_right_true();
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_A)
        ui->openGLWidget->set_camera_left_false();
    if(event->key()==Qt::Key_D)
        ui->openGLWidget->set_camera_right_false();
}

void MainWindow::updata_camera_x(float move,My_Vector3D now)
{
    trans_model.rotate(move, now);
}

//打开STL文件
void MainWindow::on_open_act_triggered()
{
    QString curPath=QCoreApplication::applicationDirPath(); //获取应用程序的路径

    //调用打开文件对话框打开一个文件
     QString aFileName=QFileDialog::getOpenFileName
             (this,"打开一个文件",curPath,"STL文件(*.stl)");
    if (aFileName.isEmpty())
    {
        return;
    }
    if(openDateSTL(aFileName))
       QMessageBox::information(this,"提示消息","文件已经打开！");
    else
        QMessageBox::information(this,"提示消息","文件打开错误！");
}

bool MainWindow::openDateSTL(QString &aFileName)
{

//    QFile aFile_2(aFileName);

//    if(!(aFile_2.open(QIODevice::ReadOnly)))
//        return false;
//    QDataStream aStream_2(&aFile_2);
//    aStream_2.setByteOrder(QDataStream::LittleEndian);   //设置为小端字节序
//    read_Binary_2(aStream_2);

    QFile aFile(aFileName);
    if(!(aFile.open(QIODevice::ReadOnly)))
        return false;
    QDataStream aStream(&aFile);
    aStream.setByteOrder(QDataStream::LittleEndian);   //设置为小端字节序

    char first;
    aStream.readRawData(&first,1); //读取第一个字节
    if(first==first_stl_ASSCII)
    {
        QTextStream strStream(&aFile);
        QString str;
        bool ascii_true=false;
        //对前5个string进行读取，检测是否存在 facet
        for(int i=0;i!=5;++i)
        {
            strStream>>str;
            if(str=="facet")
            {
                ascii_true=true;
                break;
            }
        }

        if(ascii_true)
            return read_ASCII(strStream);

        //针对开头为 s 的二进制，虽然说目前我也就遇到一个
        else
        {
            QFile aFile_2(aFileName);
            if(!(aFile_2.open(QIODevice::ReadOnly)))
                return false;
            QDataStream aStream_2(&aFile_2);
            aStream_2.setByteOrder(QDataStream::LittleEndian);   //设置为小端字节序
            char first;
            aStream_2.readRawData(&first,1); //读取第一个字节

            return read_Binary(aStream_2);
        }

    }
    else
    {
        return read_Binary(aStream);
    }
}

//读取ASCII类型数据
bool MainWindow::read_ASCII(QTextStream& aStream)
{
    //模型清空，等待接收新的数据
    model.clear();
    //清空变换矩阵
    ui->openGLWidget->m_transform_restart();
    trans_model.restart();

    QString str;
    int i=0;
    QVector<float> vec_size{-9999.0f,9999.0f,-9999.0f,9999.0f,-9999.0f,9999.0f};
    while(!aStream.atEnd())
    {
        float x,y,z;
        aStream>>str>>x>>y>>z;
        aStream>>str;
        aStream>>str;
        for(int j=0;j!=3;++j)
        {
            float x1,y1,z1;
            aStream>>str>>x1>>y1>>z1;

            if(x1>vec_size[0])
                vec_size[0]=x1;
            else if(x1<vec_size[1])
                vec_size[1]=x1;

            if(y1>vec_size[2])
                vec_size[2]=y1;
            else if(y1<vec_size[3])
                vec_size[3]=y1;

            if(z1>vec_size[4])
                vec_size[4]=z1;
            else if(z1<vec_size[5])
                vec_size[5]=z1;

            vertexs_around[My_Vector3D(x1,y1,z1)].push_back(i);
            model.push_back(Vertex(x1,y1,z1,x,y,z));
        }

        aStream>>str;
        aStream>>str;
        aStream>>str;
        if(str=="endsolid")
            break;
        ++i;
    }
    if(str=="endsolid")
    {      
        model.set_size(vec_size);
        x_n=x_now=model.size_X();
        y_n=y_now=model.size_Z();
        z_n=z_now=model.size_Y();

        model_translate();  //设置模型变换，使其位于框体正中心，若模型较大，则进行缩小

        ui->openGLWidget->input_model(model);
        emit new_model_data(true);         //发送信号，通知opengl可以进行渲染
        //save_data_vertex_data();

        //模型尺寸缩放设置
        set_models_size();
        //存储每个mesh周围的meshs
        mesh_around_push();

        return true;
    }
    else
    {
        return false;
    }

}

//读取二进制类型数据
bool MainWindow::read_Binary(QDataStream& aStream)
{
    //跳过开头文件信息
    char first;
    for(size_t i=0;i!=79;++i)
    {
        aStream.readRawData(&first,1);
    }
    vertexs_around.clear();
    meshs_around.clear();
    model.clear();       //模型清空，等待接收新的数据
    //清空变换矩阵
    ui->openGLWidget->m_transform_restart();
    trans_model.restart();

    qint32 num_mesh,i=0;
    aStream.readRawData((char*)&num_mesh,sizeof(qint32));           //..找了一天的BUG。。
    qint16 last_stl;
    float x,y,z;
    float x1,y1,z1;
    QVector<float> vec_size{-9999.0f,9999.0f,-9999.0f,9999.0f,-9999.0f,9999.0f};
    for(;i!=num_mesh;++i)
    {
        aStream.readRawData( (char*) &x, 4 );
        aStream.readRawData( (char*) &y, 4 );
        aStream.readRawData( (char*) &z, 4 );
        for(size_t j=0;j!=3;++j)
        {
            aStream.readRawData( (char*) &x1, 4 );
            aStream.readRawData( (char*) &y1, 4 );
            aStream.readRawData( (char*) &z1, 4 );

            //计算hash 的K值
//            float k_xyz=(x1*1000)/23+(y1*1000)/19+(z1*1000)/17;
//            k.push_back(k_xyz);
//            QVector<float> vec{x1,y1,z1};
//            vertexs_around[vec].push_back(i);

            if(x1>vec_size[0])
                vec_size[0]=x1;
            else if(x1<vec_size[1])
                vec_size[1]=x1;

            if(y1>vec_size[2])
                vec_size[2]=y1;
            else if(y1<vec_size[3])
                vec_size[3]=y1;

            if(z1>vec_size[4])
                vec_size[4]=z1;
            else if(z1<vec_size[5])
                vec_size[5]=z1;

            vertexs_around[My_Vector3D(x1,y1,z1)].push_back(i);
            model.push_back(Vertex(x1,y1,z1,x,y,z));
        }
        aStream.readRawData((char*)&last_stl,2);
    }
    if(i==num_mesh)
    {
        model.set_size(vec_size);
        x_n=x_now=model.size_X();
        y_n=y_now=model.size_Z();
        z_n=z_now=model.size_Y();

        model_translate();  //设置模型变换，使其位于框体正中心，若模型较大，则进行缩小

        ui->openGLWidget->input_model(model);
        emit new_model_data(true);         //发送信号，通知opengl可以进行渲染
        //save_data_vertex_data();

        //模型尺寸缩放设置
        set_models_size();
        //存储每个mesh周围的meshs
        mesh_around_push();

        return true;
    }
    else
        return false;
}

//设置模型变换，使其位于框体正中心，若模型较大，则进行缩小
void MainWindow::model_translate()
{
    trans_model.setScale(0.1f,0.1f,0.1f);//基础变形，缩小10倍
    trans_model.setTranslation_mid(-model.mid_x(),-model.mid_y(),-model.mid_z()); //将模型移动到正中心变换

    float max_model_size=0;  //模型X-Z轴中最大的
    if(model.size_X()>model.size_Z())
        max_model_size=model.size_X();
    else
        max_model_size=model.size_Z();

    float num_trans;         //缩放倍数
    if(150.0f/max_model_size < 200.0f/model.size_Y())
        num_trans=150.0f/max_model_size;
    else
        num_trans=200.0f/model.size_Y();

    if(num_trans<1)
    {
        trans_model.scale(num_trans,num_trans,num_trans);//设置缩放比例
        auto distan_y=(1-num_trans)*model.mid_y();
        auto distan_x=(1-num_trans)*model.mid_x();
        auto distan_z=(1-num_trans)*model.mid_z();
        trans_model.translate_mid(distan_x,distan_y,distan_z);

        zoom_now=num_trans;   //缩放参数设置
        trans_model.setTranslation(0.0f,model.min_y(zoom_now),0.0f); //使模型置于底部
        y_mid+=model.min_y(num_trans);
    }
    else
    {
        zoom_now=1.0f;
        trans_model.setTranslation(0.0f,model.min_y(zoom_now),0.0f); //使模型置于底部
        y_mid+=model.min_y(zoom_now);
    }
    //将变换传递至opengl widget
    ui->openGLWidget->set_transform(trans_model);
}

//尺寸界面显示隐藏
void MainWindow::on_pushButton_clicked()
{
    if(ui->groupBox->isVisible())
        ui->groupBox->setVisible(false);
    else
    {
        ui->groupBox->setVisible(true);
        ui->groupBox_2->setVisible(false);
    }
}

//角度界面显示
void MainWindow::on_pushButton_2_clicked()
{
    if(ui->groupBox_2->isVisible())
        ui->groupBox_2->setVisible(false);
    else
    {
        ui->groupBox_2->setVisible(true);
        ui->groupBox->setVisible(false);
    }

}

//改变界面缩放比例的值，使模型大小改变
void MainWindow::on_doubleSpinBox_Zoom_valueChanged(double arg1)
{
    if(flag)
    {
        flag=false;
        auto old_zoom=zoom_now;
        zoom_now=arg1;
        updata_xyz();

        ui->doubleSpinBox_H->setValue(z_now);
        ui->doubleSpinBox_X->setValue(x_now);
        ui->doubleSpinBox_Y->setValue(y_now);
        flag=true;

        mid_scale_changge(old_zoom);
        ui->openGLWidget->set_transform(trans_model);
    }

}
//改变界面X Y Z的值，使模型大小改变
void MainWindow::on_doubleSpinBox_X_valueChanged(double arg1)
{
    if(flag)
    {
        flag=false;
        auto old_zoom=zoom_now;
        zoom_now=arg1/model.size_X();
        updata_xyz();

        ui->doubleSpinBox_H->setValue(z_now);
        ui->doubleSpinBox_Y->setValue(y_now);
        ui->doubleSpinBox_Zoom->setValue(zoom_now);
        flag=true;

        mid_scale_changge(old_zoom);
        ui->openGLWidget->set_transform(trans_model);
    }
}
void MainWindow::on_doubleSpinBox_Y_valueChanged(double arg1)
{
    if(flag)
    {
        flag=false;
        auto old_zoom=zoom_now;
        zoom_now=arg1/model.size_Z();
        updata_xyz();

        ui->doubleSpinBox_H->setValue(z_now);
        ui->doubleSpinBox_X->setValue(x_now);
        ui->doubleSpinBox_Zoom->setValue(zoom_now);

        flag=true;
        if(old_zoom!=zoom_now)
            mid_scale_changge(old_zoom);
        ui->openGLWidget->set_transform(trans_model);
    }
}
void MainWindow::on_doubleSpinBox_H_valueChanged(double arg1)
{
    if(flag)
    {
        flag=false;
        auto old_zoom=zoom_now;
        zoom_now=arg1/model.size_Y();
        updata_xyz();

        ui->doubleSpinBox_X->setValue(x_now);
        ui->doubleSpinBox_Y->setValue(y_now);
        ui->doubleSpinBox_Zoom->setValue(zoom_now);
        flag=true;

        if(old_zoom!=zoom_now)
            mid_scale_changge(old_zoom);
        ui->openGLWidget->set_transform(trans_model);
    }
}

//缩放后中心位置改变函数
void MainWindow::mid_scale_changge(float& old_zoom)
{
    auto new_old=zoom_now/old_zoom;

    trans_model.scale(new_old,new_old,new_old);
    auto changezoom=old_zoom-zoom_now;
    auto distan_y=(changezoom)*model.mid_y();
    auto distan_x=(changezoom)*model.mid_x();
    auto distan_z=(changezoom)*model.mid_z();
    trans_model.translate_mid(distan_x,distan_y,distan_z);   //使中心位置不变

    auto y_chan=model.min_y(zoom_now)-model.min_y(old_zoom);
    y_mid-=y_chan;
    trans_model.translate(0.0f,y_chan,0.0f);           //移动至底部
    ui->openGLWidget->set_Y_move_to_bottom(y_chan);
}
//设置界面显示
void MainWindow::set_models_size()
{
    flag=false;
    ui->doubleSpinBox_H->setValue(z_now);
    ui->doubleSpinBox_X->setValue(x_now);
    ui->doubleSpinBox_Y->setValue(y_now);
    ui->doubleSpinBox_Zoom->setValue(zoom_now);
    flag=true;
}

void MainWindow::on_pushButton_up_clicked()
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto x_now=QMatrix3x3_model(my_x,up_change);
    auto rota=ui->openGLWidget->get_rotation();

    trans_model.setRotation(rota);
    trans_model.rotate(90.0f,x_now);
    ui->openGLWidget->set_normal_trans(90.0f,my_x);
    ui->openGLWidget->set_transform(trans_model);
}
void MainWindow::on_pushButton_down_clicked()
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto x_now=QMatrix3x3_model(my_x,up_change);
    auto rota=ui->openGLWidget->get_rotation();

    trans_model.setRotation(rota);
    trans_model.rotate(270.0f,x_now);
    ui->openGLWidget->set_normal_trans(270.0f,my_x);
    ui->openGLWidget->set_transform(trans_model);
}

void MainWindow::on_pushButton_right_clicked()
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto y_now=QMatrix3x3_model(my_y,up_change);
    auto rota=ui->openGLWidget->get_rotation();

    trans_model.setRotation(rota);
    trans_model.rotate(90.0f,y_now);
    ui->openGLWidget->set_normal_trans(90.0f,my_y);
    ui->openGLWidget->set_transform(trans_model);
}
void MainWindow::on_pushButton_left_clicked()
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto y_now=QMatrix3x3_model(my_y,up_change);
    auto rota=ui->openGLWidget->get_rotation();

    trans_model.setRotation(rota);
    trans_model.rotate(270.0f,y_now);
    ui->openGLWidget->set_normal_trans(270.0f,my_y);
    ui->openGLWidget->set_transform(trans_model);
}

void MainWindow::on_pushButton_front_clicked()
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto x_now=QMatrix3x3_model(my_x,up_change);
    auto rota=ui->openGLWidget->get_rotation();

    trans_model.setRotation(rota);
    trans_model.rotate(180.0f,x_now);
    ui->openGLWidget->set_normal_trans(180.0f,my_x);
    ui->openGLWidget->set_transform(trans_model);
}
void MainWindow::on_pushButton_back_clicked()
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto y_now=QMatrix3x3_model(my_y,up_change);
    auto rota=ui->openGLWidget->get_rotation();

    trans_model.setRotation(rota);
    trans_model.rotate(180.f,y_now);
    ui->openGLWidget->set_normal_trans(180.0f,my_y);
    ui->openGLWidget->set_transform(trans_model);
}

//模型旋转
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    auto up_change=ui->openGLWidget->transfrom_0();  //获得框的变换矩阵
    auto x_now=QMatrix3x3_model(my_x,up_change);     //求得变换后的旋转轴

    trans_model.rotate(value - x_last,x_now);         //依据该轴旋转
    ui->openGLWidget->normal_trans(value-x_last,my_x);   //将旋转变换传至normal_change
    x_last=value;

    ui->openGLWidget->set_transform(trans_model);     //更新opengl内变换
}
void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto z_now=QMatrix3x3_model(my_z,up_change);

    trans_model.rotate(value - z_last,z_now);
    ui->openGLWidget->normal_trans(value - z_last,my_z);
    z_last=value;

    ui->openGLWidget->set_transform(trans_model);
}
void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    auto up_change=ui->openGLWidget->transfrom_0();
    auto y_now=QMatrix3x3_model(my_y,up_change);

    trans_model.rotate(value-y_last,y_now);
    ui->openGLWidget->normal_trans(value-y_last,my_y);
    y_last=value;

    ui->openGLWidget->set_transform(trans_model);
}
My_Vector3D MainWindow::QMatrix3x3_model(My_Vector3D vec,QMatrix3x3 & matrix )
{
    return My_Vector3D
            (vec.x() * matrix(0,0) + vec.y() * matrix(0,1) +vec.z()* matrix(0,2),
             vec.x() * matrix(1,0) + vec.y() * matrix(1,1) +vec.z()* matrix(1,2),
             vec.x() * matrix(2,0) + vec.y() * matrix(2,1) +vec.z()* matrix(2,2));
}

//检测需要支撑的位置
void MainWindow::on_pushButton_4_clicked()
{
    ui->openGLWidget->update_normal();
}
//需支撑位置显示
void MainWindow::on_checkBox_clicked(bool checked)
{
   ui->openGLWidget->set_show_red(checked);
}

//视角重置
void MainWindow::on_pushButton_3_clicked()
{
    ui->openGLWidget->camera_restart(zoom_now);
}
//模型大小重置
void MainWindow::on_pushButton_5_clicked()
{
    ui->doubleSpinBox_Zoom->setValue(1.0);
}

//载入支撑样本
void MainWindow::load_sample_models()
{
     QString aFileName="column.stl";

     QFile aFile(aFileName);
     aFile.open(QIODevice::ReadOnly);

     QDataStream aStream(&aFile);
     aStream.setByteOrder(QDataStream::LittleEndian);   //设置为小端字节序

     char first;
     for(size_t i=0;i!=80;++i)
     {
         aStream.readRawData(&first,1);
     }

     sample_model.clear();
     qint32 num_mesh,i=0;
     aStream.readRawData((char*)&num_mesh,sizeof(qint32));           //..找了一天的BUG。。
     qint16 last_stl;
     float x,y,z;
     float x1,y1,z1;
     QVector<float> vec_size{-9999.0f,9999.0f,-9999.0f,9999.0f,-9999.0f,9999.0f};
     for(;i!=num_mesh;++i)
     {
         aStream.readRawData( (char*) &x, 4 );
         aStream.readRawData( (char*) &y, 4 );
         aStream.readRawData( (char*) &z, 4 );
         for(size_t j=0;j!=3;++j)
         {
             aStream.readRawData( (char*) &x1, 4 );
             aStream.readRawData( (char*) &y1, 4 );
             aStream.readRawData( (char*) &z1, 4 );

             if(x1>vec_size[0])
                 vec_size[0]=x1;
             else if(x1<vec_size[1])
                 vec_size[1]=x1;

             if(y1>vec_size[2])
                 vec_size[2]=y1;
             else if(y1<vec_size[3])
                 vec_size[3]=y1;

             if(z1>vec_size[4])
                 vec_size[4]=z1;
             else if(z1<vec_size[5])
                 vec_size[5]=z1;

             sample_model.push_back(Vertex(x1,y1,z1,x,y,z));
         }
         aStream.readRawData((char*)&last_stl,2);
     }
     if(i==num_mesh)
     {
         sample_model.set_size(vec_size);
         ui->openGLWidget->load_sample_models(sample_model);
     }
}

//计算支撑位置并生成
void MainWindow::on_pushButton_6_clicked()
{
    ui->openGLWidget->set_draw_suppports_true(zoom_now);
}

void MainWindow::show_supperts_num(int num)
{
    ui->spinBox_4->setValue(num);
}

//存储每个mesh周围的meshs
void MainWindow::mesh_around_push()
{
    int count_1=0,count_2=0,count_3=0;
    QVector<Vertex>& meshs=model.vertexs_of_model();         //顶点数组
    for(int i=0,z=0;i!=model.size_vertex();i=i+3,++z)
    {
        QVector<int> ver_1=vertexs_around[meshs[i].position()];
        QVector<int> ver_2=vertexs_around[meshs[i+1].position()];
        QVector<int> ver_3=vertexs_around[meshs[i+2].position()];

        QMap<int,int> map_1;
        QVector<int> vec;
        for(auto &j:ver_1)
            map_1[j]++;
        for(auto &j:ver_2)
        {
            map_1[j]++;
            if(j!=z && map_1[j]!=1)
                vec.push_back(j);
        }
        for(auto &j:ver_3)
        {
            map_1[j]++;
            if(j!=z && map_1[j]!=1)
                vec.push_back(j);
        }
        if(vec.size()==3)
            count_3++;
        else if(vec.size()==2)
            count_2++;
        else if(vec.size()==1)
            count_1++;
        meshs_around.push_back(vec);
    }
    ui->openGLWidget->set_mesh_around(meshs_around);
    ui->spinBox->setValue(count_1);
    ui->spinBox_2->setValue(count_2);
    ui->spinBox_3->setValue(count_3);
    ui->spinBox_5->setValue(model.size_vertex()/3);

}
