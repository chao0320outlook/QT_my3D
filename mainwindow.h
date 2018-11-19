#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QDataStream>
#include "model.h"
#include "transform_3d.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QVector3D QMatrix3x3_model(QVector3D  vec,QMatrix3x3 & matrix );

    bool flag=true;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool read_ASCII(QTextStream& aStream);
    bool read_Binary(QDataStream& aStream);
    bool read_Binary_2(QDataStream& aStream);
    bool save_data_vertex_data();

    bool openDateSTL(QString &aFileName);
    void set_models_size();
    void model_size();
    void model_translate();
    void mid_scale_changge(float& old_zoom);
    void load_sample_models();

    void updata_xyz()
    {
        z_now=zoom_now*z_n;
        x_now=zoom_now*x_n;
        y_now=zoom_now*y_n;
    }

signals:
    void new_model_data(bool flag);

private:
    Ui::MainWindow *ui;
    Model model;
    Model sample_model;
    Transform3D trans_model;

    int x_last=0,y_last=0,z_last=0;
    //Z为高度
    float x_n=0.0f,y_n=0.0f,z_n=0.0f;  //原始数据
    float x_now=0.0f,y_now=0.0f,z_now=0.0f,zoom_now=1.0f;    //X Y Z Zoom 值
    float x_mid=0.0f,y_mid=0.0f,z_mid=0.0f;  //实时中间点


private slots:
    //自定义槽函数
    void keyPressEvent(QKeyEvent *event);

    void updata_camera_x(float move,QVector3D now);

    void keyReleaseEvent(QKeyEvent *event);

    void on_open_act_triggered();
    void on_pushButton_clicked();

    void on_doubleSpinBox_Zoom_valueChanged(double arg1);
    void on_doubleSpinBox_X_valueChanged(double arg1);
    void on_doubleSpinBox_Y_valueChanged(double arg1);
    void on_doubleSpinBox_H_valueChanged(double arg1);
    void on_pushButton_2_clicked();
    void on_pushButton_up_clicked();
    void on_pushButton_right_clicked();
    void on_pushButton_front_clicked();
    void on_pushButton_down_clicked();
    void on_pushButton_left_clicked();
    void on_pushButton_back_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_3_valueChanged(int value);
    void on_pushButton_4_clicked();
    void on_checkBox_clicked(bool checked);
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
};

#endif // MAINWINDOW_H
