#ifndef FIREWORK_H
#define FIREWORK_H
#include <QVector3D>
#include "particle.h"

#include "firework.h"
#include <QOpenGLFunctions>

class firework
{
private:
    int time_c = 0;
    QList<Particle> particles;
    int N;
    
    QVector3D light_color;       // 点光源的颜色
    bool explosion_active = false;  // 是否激活了光源

public:
    firework(int N, QPoint mousePosition);
    void inc_time();
    void activate_explosion_light(const QVector3D& color);  // 激活光源
    void deactivate_explosion_light();  // 取消光源
    int particles_coordinate_size(int i);
    int particles_size();
    void recount_particles(int i);
    QVector3D particle_coordinate(int i, int j);
    QVector3D get_particle_color(int i, int j);
    float get_alpha(int i, int j);
    QVector3D color;
    QVector3D explosion_center;  // 爆炸中心
    // OpenGL渲染时使用的函数
    void render_explosion_light();  // 渲染爆炸光源

    void setBlinnPhongLighting();
};
#endif // FIREWORK_H
