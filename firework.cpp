#include "firework.h"
#include <QOpenGLFunctions>
#include <QRandomGenerator>
firework::firework(int N, QPoint mousePosition)
{
    color.setX((rand() % 256) / 100.0);
    color.setY((rand() % 256) / 100.0);
    color.setZ((rand() % 256) / 100.0);

    float x_mouse = static_cast<float>(mousePosition.x())-525.0;
    float y_mouse = static_cast<float>(mousePosition.y())-260.0;
    float z = static_cast<float>(mousePosition.y()) - 260.0;

    float y = -y_mouse/20;
    float x = x_mouse / 20 *1.3;
    //z = -x / 30;

    z = y_mouse / 60 - x / 30;
    //y = -y / 60;
    //z = z / 60;
    printf("x=%lf,y=%lf,z=%lf\n", x, y,z);
    explosion_center = QVector3D(x, y, 0);

    QPoint sPoint2 = QCursor::pos();

    for (int i = 0; i < N; i++) {
        //float offsetX = QRandomGenerator::global()->bounded(-5, 5);
        //float offsetY = QRandomGenerator::global()->bounded(-5, 5);
        //float offsetZ = QRandomGenerator::global()->bounded(50, 60);

        QVector3D particlePosition = explosion_center;// +QVector3D(offsetX, offsetY, offsetZ);

        particles.append(Particle(i, particlePosition.x(), particlePosition.y(), particlePosition.z(), color));
    }
    
}

void firework::inc_time() {
    time_c++;
}



int firework::particles_coordinate_size(int i)
{
    return particles[i].coordinate.size();
}

int firework::particles_size()
{
    return particles.size();
}

void firework::recount_particles(int i)
{
    particles[i].recount_points(time_c);  // 更新粒子位置
}

QVector3D firework::particle_coordinate(int i, int j)
{
    return particles[i].coordinate.at(j);
}

QVector3D firework::get_particle_color(int i, int j)
{
    return particles[i].get_color();  // 获取粒子的颜色
}

float firework::get_alpha(int i, int j)
{
    int n = particles[i].coordinate.size();
    float del = 1 / (float)n;
    if (!particles[i].waiting_for_death()) {
        return j * del;
    }
    else {
        return (n - j) * del;
    }
}
