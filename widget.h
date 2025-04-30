#ifndef WIDGET_H
#define WIDGET_H
#include <windows.h>

#include<QCoreApplication>
#include <QOpenGLWidget>
#include<QOpenGLTexture>
#include <QVector3D>
#include <QVector4D>
#include<QOpenGLShaderProgram>
#include <QTimer>
#include <QMouseEvent>

#include "firework.h"
#include "ObjModel.h"


class Widget : public QOpenGLWidget {

private:
    QList <firework> fire;
    ObjModel* objModel;
    ObjModel* objModel1;
    ObjModel* objModel2;
    int N = 50;
    int time_counter = 0;
    float axisSize = 3.5, arrowSize = 0.1;



public:
    QTimer timer;

    float angleX, angleZ, angleY;
    float max_rad = 2.5;

    float pitch=0.0f, yaw=90.0f;       //俯仰角，偏航角偏移角度
    QVector3D* worldUp;
    QVector3D* cameraPosition;
    QVector3D* cameraLookat;//相机看的点坐标
    QVector3D* right;
    QVector3D* cameraUp;
    QVector3D* figurePosition;
    QVector3D* figureScale;
    QOpenGLTexture* skyboxTextures[6];
    QOpenGLShaderProgram* shaderProgram;
    

    Widget(QWidget*);
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void add_new_firework(QPoint pos);
    void setPerspectiveProjection();
    void drawAxises();
    void setLight();
    void recountPoints();
    void scence_rotate(int dx, int dy, int dz);
    void camera_rotate(float rotation_degree, float up);
    void camera_move(float x, float y, float z);


    void camera_move(int mode, float step);
    void drawSkybox();
    void loadSkyboxTextures();


};

#endif // WIDGET_H
