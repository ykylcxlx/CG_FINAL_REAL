#include "widget.h"
#include <QDebug>
#include <QString>
#include <QVector3D>
#include <Windows.h>  // Make sure this is included first  
#include <GL/glu.h>
#include <QDebug>
#include <QOpenGLWidget>
#include <qmath.h>

int cnt = 0;
Widget::Widget(QWidget* parent = 0) : QOpenGLWidget(parent)
{

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(10); //在这个例子中，update() 会被每16毫秒（大约60帧每秒）调用一次。

    angleX = 0, angleY = 0, angleZ = 0;
    cameraPosition = new QVector3D(3, 2, 5);
    cameraUp = new QVector3D(0, 1, 0);
    figurePosition = new QVector3D(0, 0, 0);
    figureScale = new QVector3D(1, 1, 1);
    cameraLookat = new QVector3D(0, 0, 0);

    QPoint pos(rand()%10000 , rand()%10000 );
    fire.append(firework(50, pos));
    recountPoints();
}

void Widget::initializeGL()


{
    worldUp = new QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D front = *cameraLookat - *cameraPosition;
    yaw = atan2(front.x(), front.z()) - 90.0f;
    pitch = atan2(front.y(), sqrt(front.x() * front.x() + front.z() * front.z()));
    right = new QVector3D(0.0, 0.0, 0.0);
    *right = QVector3D::crossProduct(front, *worldUp).normalized();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 黑色背景
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);

    loadSkyboxTextures(); // 加载天空盒纹理

    setLight(); //开启光照: 如果这句话注释掉，烟花就会是黑色
    setPerspectiveProjection();

    objModel = new ObjModel(); //房子
    //if (!objModel->loadFromFile("./objs/Farmhouse.obj", "./textures/Farmhouse.jpg",1)) {
        //qWarning() << "Failed to load OBJ model";
    //}
    objModel1 = new ObjModel(); //月亮
    if (!objModel1->loadFromFile("./objs/Moon.obj", "./textures/Moon.png",2)) {
        qWarning() << "Failed to load OBJ model";
    }
    objModel2 = new ObjModel(); //帆船
    if (!objModel2->loadFromFile("./objs/fantasy.obj", "./textures/fantasy.jpg",3)) {
        qWarning() << "Failed to load OBJ model";
    }
}


void Widget::resizeGL(int nWidth, int nHeight)
{
    glViewport(0, 0, nWidth, nHeight);
}


void Widget::recountPoints() {
    glPushMatrix();
    //渲染多个火花或粒子系统的粒子，每个粒子都有位置、颜色和透明度。
    glPointSize(4);
    glBegin(GL_POINTS);

    for (int f = 0; f < fire.size(); f++) {
        //遍历每一个粒子
        if (timer.isActive())
        {
            fire[f].inc_time();
        }

        for (int i = 0; i < fire[f].particles_size(); i++) {
            fire[f].recount_particles(i);
            int n = fire[f].particles_coordinate_size(i);
            for (int j = 0; j < n; j++) {
                QVector3D newcoord = fire[f].particle_coordinate(i, j);
                fire[0].explosion_center = newcoord;
                QVector3D color = fire[f].get_particle_color(i, j);
                fire[0].color = color;
                glColor4f(color.x(), color.y(), color.z(), fire[f].get_alpha(i, j));
                glVertex3f(newcoord.x(), newcoord.y(), newcoord.z());
            }
        }
    }
    glEnd();
    glPopMatrix();
}
void Widget::add_new_firework(QPoint mousePosition) {
    int c = fire.size();
    fire.append(firework(50, mousePosition));
}


void Widget::setLight() {

    //material

    GLfloat material_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat shine[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);


    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 0.0 };//цвет фигуры
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

    GLfloat position[] = { 0.0, 2.5, 20.0, 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, position);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.09);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
}

void Widget::drawAxises() {
    glPushMatrix();
    //glDisable(GL_LIGHTING);
    //х
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(-axisSize, 0.0, 0.0);
    glVertex3f(axisSize, 0.0, 0.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(axisSize, 0.0, 0.0);
    glVertex3f(axisSize - arrowSize * 3, arrowSize, 0.0);
    glVertex3f(axisSize - arrowSize * 3, -arrowSize, 0.0);
    glEnd();
    //у
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0, -axisSize, 0.0);
    glVertex3f(0.0, axisSize, 0.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, axisSize, 0.0);
    glVertex3f(-arrowSize, axisSize - arrowSize * 3, 0.0);
    glVertex3f(arrowSize, axisSize - arrowSize * 3, 0.0);
    glEnd();
    //z 
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -axisSize);
    glVertex3f(0.0, 0.0, axisSize);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, axisSize);
    glVertex3f(-arrowSize, 0.0, axisSize - arrowSize * 3);
    glVertex3f(arrowSize, 0.0, axisSize - arrowSize * 3);
    glEnd();

    //glEnable(GL_LIGHTING);
    glPopMatrix();
}

void Widget::setPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0, 1, 0.1, 1000);//如果近裁剪面设置过近，可能导致一些物体被视锥裁剪掉。
    //gluLookAt(cameraPosition->x(), cameraPosition->y(), cameraPosition->z(), 0, 3.0, 0, 0, 1, 0);
    //gluLookAt(cameraPosition->x(), cameraPosition->y(), cameraPosition->z(), 0, 0, 0, cameraUp->x(), cameraUp->y(), cameraUp->z());
    gluLookAt(cameraPosition->x(), cameraPosition->y(), cameraPosition->z(), cameraLookat->x(), cameraLookat->y(), cameraLookat->z(), cameraUp->x(), cameraUp->y(), cameraUp->z());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    update();

}

void Widget::scence_rotate(int dx, int dy, int dz) {
    angleX += dx % 361;
    angleY += dy % 361;
    angleZ += dz % 361;
    setPerspectiveProjection();
}
float radian(float x) {
    float radians = x * M_PI / 180.0f;
    return radians;
}

void Widget::camera_rotate(float xOffset, float yOffset)
{

    //更新俯仰角，偏航角角度
    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    //更新摄像机坐标系
    QVector3D tmp(.0f,.0f,.0f);
    tmp.setX(std::cos(radian(yaw)) * std::cos(radian(pitch)));
    tmp.setY(std::sin(radian(pitch)));
    tmp.setZ(std::sin(radian(yaw)) * std::cos(radian(pitch)));
    tmp.normalize();
    QVector3D front = tmp;
    *right = QVector3D::crossProduct(front, *worldUp).normalized();
    *cameraUp = QVector3D::crossProduct(*right, front).normalized();
    //right = normalize(cross(front, worldUp));
    *cameraLookat = *cameraPosition + front;

}
//void Widget::camera_rotate(int rotation_degree, float up) {
    //// 定义旋转的半径和中心点
    //QVector3D center(0, 0, 0); // 假设围绕 (0, 0, 0) 旋转
    //float radius = 6; // 计算摄像机到中心点的距离

    //// 当前xz角度
    //static float currentAngle = 0.0f; // 用于累积旋转角度
    //currentAngle += rotation_degree;
    //// 将角度转换为弧度
    //float radians = currentAngle * M_PI / 180.0f;

    //// 计算新的水平位置
    //float newX = center.x() + radius * sin(radians);
    //float newZ = center.z() + radius * cos(radians);

    //// 更新摄像机的Y轴（上下视角）
    //float newY = cameraPosition->y() + up;  // 上下方向的变化

    //// 如果上下角度超过某个范围（比如90度），则限制
    //if (newY > 10.0f) newY = 10.0f;
    //if (newY < -10.0f) newY = -10.0f;

    //// 更新摄像机位置
    //cameraPosition->setX(newX);
    //cameraPosition->setZ(newZ);
    //cameraPosition->setY(newY);  // 直接更新Y坐标


    //// 需要更新视图矩阵或调用 update()
    //update();

//
//
//}

void Widget::camera_move(float x, float y, float z) {

    
    // 更新相机的位置
    float x0 = cameraPosition->x();
    float y0 = cameraPosition->y();
    float z0 = cameraPosition->z();
    //printf("x0=%lf, y0=%lf, z0=%lf\n", x0,y0,z0);
    cameraPosition->setX(x0+x);
    cameraPosition->setZ(y0+y);
    cameraPosition->setY(z0+z);  // 直接更新Y坐标

    //update(); // 刷新窗口以重新绘制
}

void Widget::camera_move(int mode,float step) {
    QVector3D front = *cameraLookat - *cameraPosition;
    // 更新相机的位置
    float x0 = cameraPosition->x();
    float y0 = cameraPosition->y();
    float z0 = cameraPosition->z();
    QVector3D delta;
    switch (mode)
    {
    case 0:
        //cameraPosition->setX(x0 + front.x() * step);
        //cameraPosition->setZ(y0 + front.y() * step);
        //cameraPosition->setY(z0 + front.z() * step);  // 直接更新Y坐标
        delta = { front.x() * step, front.y() * step, front.z() * step };
        *cameraPosition += delta;
        *cameraLookat += delta;
        break;
    case 1:
        delta = { front.x() * step, front.y() * step, front.z() * step };
        *cameraPosition -= delta;
        *cameraLookat -= delta;
        break;
    case 2:
        //cameraPosition->setX(x0 + right->x() * step);
        //cameraPosition->setZ(y0 + right->y() * step);
        //cameraPosition->setY(z0 + right->z() * step);  // 直接更新Y坐标
        delta = { right->x() * step, right->y() * step, right->z() * step };
        *cameraPosition += delta;
        *cameraLookat += delta;
        break;
    case 3:
        delta = { right->x() * step, right->y() * step, right->z() * step };
        *cameraPosition -= delta;
        *cameraLookat -= delta;
        break;
    case 5://up
        delta = { cameraUp->x() * step, cameraUp->y() * step, cameraUp->z() * step };
        *cameraPosition += delta;
        *cameraLookat += delta;
        break;
    case 6://down
        delta = { cameraUp->x() * step, cameraUp->y() * step, cameraUp->z() * step };
        *cameraPosition -= delta;
        *cameraLookat -= delta;
        break;


    default:
        break;
    }

    //update(); // 刷新窗口以重新绘制
}





void Widget::loadSkyboxTextures() {
    QString textureFiles[6] = {
        "./textures/space1.png",  // 右
        "./textures/space1.png",   // 左
        "./textures/space2.png",    // 上
        "./textures/space3.jpg", // 下
        "./textures/space1.png",  // 前
        "./textures/space1.png"    // 后
    };

    // 使用 QOpenGLTexture 来加载纹理
    for (int i = 0; i < 6; i++) {
        QImage img(textureFiles[i]);
        // 获取图像的格式
        QImage::Format format = img.format();

        // 输出图像的格式
        switch (format) {
        case QImage::Format_RGB888:
            qDebug() << "RGB format (no alpha channel)";
            break;
        case QImage::Format_RGBA8888:
            qDebug() << "RGBA format (with alpha channel)";
            break;
        case QImage::Format_RGB32:
            qDebug() << "RGB32 format (with alpha, 32-bit)";
            break;
        case QImage::Format_ARGB32:
            qDebug() << "ARGB32 format (with alpha, 32-bit)";
            break;
        default:
            qDebug() << "Other format";
        }
        img = img.convertToFormat(format);  // 确保纹理格式正确

        // 创建一个纹理对象并绑定
        QOpenGLTexture* texture = new QOpenGLTexture(img);
        texture->setMinificationFilter(QOpenGLTexture::Linear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        texture->setWrapMode(QOpenGLTexture::ClampToEdge);

        skyboxTextures[i] = texture;  // 保存纹理对象
    }
}


void Widget::paintGL() {
    cnt++;
    //paintGL() 函数是由 Qt 的 OpenGL 渲染机制自动调用的。
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 设置照相机
    setPerspectiveProjection();

    // 绘制坐标轴
    drawAxises();

    // 绘制天空盒
    drawSkybox();

    //绘制粒子的函数
    recountPoints();

    //关闭全局光照
    glDisable(GL_LIGHTING);
    // 设置房子模型的缩放和位置-----------------------------------------------------------
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // 应用缩放：例如将对象缩小为原来的 0.1 倍
    glScalef(1.0f, 1.0f, 1.0f);  // 缩小

    // 应用平移：将对象放置在合理位置，例如放置在原点附近
    glTranslatef(0.0f, -80.0f, 150.0f);  // 将对象沿 z 轴平移
    QVector3D explosion_center = fire[0].explosion_center;//所有烟花的exlposion_center一样的，所以选第一个就行了
    QVector3D color = fire[0].color;
    // 绘制帆船模型
    if (objModel2) {
        //objModel2->draw3(explosion_center, color, cameraLookat);
    }
    glPopMatrix();

    // 设置月亮模型的缩放和位置-----------------------------------------------------------
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // 应用缩放：例如将对象放大为原来的 2 倍
    glScalef(1.0f, 1.0f, 1.0f);  // 暂时不变

    // 应用平移：将对象放置在合理位置，例如放置在原点附近
    glTranslatef(0.0f, 0.0f, 0.0f);  // 将对象沿 y 轴平移

    // 绘制月亮模型
    if (cnt ) {
        if (objModel1) {
            objModel1->draw2(explosion_center, color, cameraLookat);
        }

        glPopMatrix();
        // 设置帆船模型的缩放和位置-------------------------------------------------------
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // 应用缩放：例如将对象缩小为原来的 0.1 倍
        glScalef(1.0f, 1.0f, 1.0f);  // 不变，在draw里变了

        // 应用平移：将对象放置在合理位置，例如放置在原点附近
        glTranslatef(0.0f, 0.0f, 0.0f);  // 将对象沿 z 轴平移
         //制帆船模型
        if (objModel2) {
            objModel2->draw3(explosion_center, color, cameraLookat);
        }
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
    fire[0].explosion_center = { -50.0f,50.0f,50.0f };
    fire[0].color = { 0.0f,0.0f,0.0f };
    //场景变化，按鼠标左右键可以旋转场景
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);
    glTranslatef(figurePosition->x(), figurePosition->y(), figurePosition->z());
    glScalef(figureScale->x(), figureScale->y(), figureScale->z());
    glPopMatrix();
}

void Widget::drawSkybox() {
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0f,1.0f,1.0f); 
    // 禁用光照，这样天空盒就不会受光照影响
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D); // Enable texture mapping
    // 绘制六个面，定义天空盒的大小
    float size = 80.0f;  // 天空盒的大小（根据需要调整）

    // 绘制前面
    skyboxTextures[4]->bind();  // 前面纹理
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(size, size, size);
    glTexCoord2f(1.0, 0.0); glVertex3f(-size, size, size);
    glTexCoord2f(1.0, 1.0); glVertex3f(-size, -size, size);
    glTexCoord2f(0.0, 1.0); glVertex3f(size, -size, size);
    glEnd();

    // 绘制后面
    skyboxTextures[5]->bind();  // 后面纹理
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0, 0.0); glVertex3f(size, size, -size);
    glTexCoord2f(1.0, 1.0); glVertex3f(size, -size, -size);
    glTexCoord2f(0.0, 1.0); glVertex3f(-size, -size, -size);
    glEnd();

    // 绘制上面
    skyboxTextures[2]->bind();  // 上面纹理
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0, 0.0); glVertex3f(size, size, -size);
    glTexCoord2f(1.0, 1.0); glVertex3f(size, size, size);
    glTexCoord2f(0.0, 1.0); glVertex3f(-size, size, size);
    glEnd();

    // 绘制下面
    skyboxTextures[3]->bind();  // 下面纹理
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(size, -size, -size);//size, -size, -size
    glTexCoord2f(1.0, 0.0); glVertex3f(size, -size, size);
    glTexCoord2f(1.0, 1.0); glVertex3f(-size, -size, size);
    glTexCoord2f(0.0, 1.0); glVertex3f(-size, -size, -size);
    glEnd();

    // 绘制左面
    skyboxTextures[1]->bind();  // 左面纹理
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-size, size, size);
    glTexCoord2f(1.0, 0.0); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0, 1.0); glVertex3f(-size, -size, -size);
    glTexCoord2f(0.0, 1.0); glVertex3f(-size, -size, size);
    glEnd();

    // 绘制右面
    skyboxTextures[0]->bind();  // 右面纹理
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(size, size, -size);
    glTexCoord2f(1.0, 0.0); glVertex3f(size, size, size);
    glTexCoord2f(1.0, 1.0); glVertex3f(size, -size, size);
    glTexCoord2f(0.0, 1.0); glVertex3f(size, -size, -size);
    glEnd();

    glDisable(GL_TEXTURE_2D); // Enable texture mapping
    glDisable(GL_TEXTURE_2D); // Enable texture mapping
    glEnable(GL_LIGHTING);  // 恢复光照

    glPopMatrix();
}

