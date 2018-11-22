#-------------------------------------------------
#
# Project created by QtCreator 2018-09-12T20:54:52
#
#-------------------------------------------------

QT       += core gui
QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = My_3D_1
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    my_opengl.cpp \
    transform_3d.cpp \
    camera.cpp \
    input.cpp \
    mesh.cpp

HEADERS +=\
        mainwindow.h \
    my_opengl.h \
    transform_3d.h \
    camera.h \
    vertex.h \
    input.h \
    mesh.h \
    model.h \
    my_vector3d.h

FORMS +=\
    mainwindow.ui

RESOURCES += \
    shaders.qrc \
    images.qrc

