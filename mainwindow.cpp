#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <QResizeEvent>
#include <qmath.h>
#include <QDebug>
#include <QSurfaceFormat>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //这会确保即使没有按下鼠标按钮，也能检测鼠标移动事件
    setMouseTracking(true);

    ui->setupUi(this);
    ui->pushButton->setStyleSheet("border-image: url(:/fire.png) stretch;");

    QSurfaceFormat  fmt;
    //fmt.setSampleBuffers(true);
    fmt.setSamples(4);
    fmt.setSamples(16);
    QSurfaceFormat::setDefaultFormat(fmt);
    glwidget = new Widget(this);
    int width = size().width() - 245;
    int height = size().height() - 10;
    glwidget->setGeometry(235, 5, width, height);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    int width = e->size().width() - 245;
    int height = e->size().height() - 10;
    glwidget->setGeometry(235, 5, width, height);
    glwidget->resizeGL(width, height);
}




void MainWindow::on_pushButton_clicked(){
    //点击按钮产生新的烟花
    QPoint point(rand()%180+525, rand()%180+260);
    glwidget->add_new_firework(point);
}


void MainWindow::mousePressEvent(QMouseEvent *e) {

    if(e->x()<235||e->x()>815)
        return;
    if(e->y()<5||e->y()>500)
        return;
    x0 = (float) e->x();
    y0 = (float) e->y();
    glwidget->timer.stop();

    if (e->button() == Qt::LeftButton) {
        QPoint mousePosition = e->pos();
        int numberOfParticles = 100; // 你可以根据需要设置粒子数量
        glwidget->add_new_firework(mousePosition);
        // 然后触发烟花的渲染或其他逻辑
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e) {
     glwidget->timer.start(60);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    //按下鼠标可以移动烟花场景（理论上并没有调整摄像机位置）
    if(e->x()<235||e->x()>815)
        return;
    if(e->y()<5||e->y()>500)
        return;

    if (e->buttons() & Qt::LeftButton) {
        //glwidget->scence_rotate((e->y()-y0)/100.0, 0,0);//表示绕x轴旋转（因此需要鼠标竖直移动）
        //glwidget->scence_rotate(0, (e->x()-x0)/100.0, 0);
        glwidget->camera_rotate((x0-e->x())/100, (e->y()- y0) / 100.0);

    }
    if (e->buttons() & Qt::RightButton) {

        glwidget->scence_rotate(0, 0, (e->y()-y0)/100.0);
        glwidget->scence_rotate(0, (e->x()-x0)/100.0, 0);
    }
}
void MainWindow::keyPressEvent(QKeyEvent* e) {
    //按下键盘左右键可以使得照相机左右移动

    float d = 10;
    float step = 1.0;
    switch (e->key()) {
    case Qt::Key_J: glwidget->camera_rotate(-2,0); break;//左转
    case Qt::Key_L: glwidget->camera_rotate(2,0); break;//右转
    case Qt::Key_I: glwidget->camera_rotate(0, 1); break;//向上旋转
    case Qt::Key_K: glwidget->camera_rotate(0, -1); break;//下转

    //case Qt::Key_L:  glwidget->camera_move(-d, 0.0, 0.0); break;  // 向左移动
    //case Qt::Key_R: glwidget->camera_move(d, 0.0, 0.0); break;   // 向右移动
    //case Qt::Key_U: glwidget->camera_move(0.0, d, 0.0); break;   // 向上移动
    //case Qt::Key_N: glwidget->camera_move(0.0, -d, 0.0); break;  // 向下移动
    //case Qt::Key_F: glwidget->camera_move(0.0, 0.0, d); break;  // 向前移动
    //case Qt::Key_B: glwidget->camera_move(0.0, 0.0, -d); break; // 向后移动


    case Qt::Key_A:  glwidget->camera_move(3, step); break;  // 向左移动
    case Qt::Key_D: glwidget->camera_move(2, step); break;   // 向右移动
    case Qt::Key_U: glwidget->camera_move(5, step); break;   // 向上移动
    case Qt::Key_N: glwidget->camera_move(6, step); break;  // 向下移动
    case Qt::Key_W: glwidget->camera_move(0, step); break;  // 向前移动
    case Qt::Key_S: glwidget->camera_move(1, step); break; // 向后

    }
}

