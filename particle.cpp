#include "particle.h"
#include <QDebug>
#include <qmath.h>
Particle::Particle(){

}

Particle::Particle(int ID, float x_0, float y_0, float z_0, QVector3D firework_color)
{
    //��ʼ�����ӵ���ɫ
    id = ID;
    lifetime = rand()%101;
    radius = (rand()% 181)/30.0;
    velocity = radius-3 ;
    color.setX(firework_color.x());
    color.setY(firework_color.y());
    color.setZ(firework_color.z());
    x0=x_0;
    y0=y_0;
    z0=z_0;

}

float Particle::get_velocity(){
    return velocity;
}

QVector3D Particle::get_color(){
    return color;
}

int Particle::get_lifetime(){
    return lifetime;
}

int Particle::get_id(){
    return id;
}


bool Particle::waiting_for_death(){
    return is_dying;
}

void Particle::recount_points(int time){
    //�������ÿ�����ӵİ���켣����Ŷż��λ��λ��
    float x, y, z, step = 2 * M_PI / 100.0;
    float Angle = (time%101)*step;
    coordinate.clear();
    if(is_dying)
        return;
    if(lifetime<time)
        is_dying = true;

    int length = 25;
    //length��ʾ��β����
    for (int i = 0; (i < length)&&(Angle<=M_PI);i++){
        x = radius+radius * cos(Angle+M_PI);
        y = radius * sin(Angle);
        z = 0;
        //z = radius * sin(Angle);  // ʹZ��Ҳ���˶�
        coordinate.append(QVector3D(x, y, z));
        Angle+=step/(5-velocity/5.0);
    }

    int n = coordinate.size();

    for(int i = 0; i< n; i++){
        QVector3D newcoord = coordinate.at(i);
        coordinate[i].setX(newcoord.x()*cos(id*7.2)+newcoord.z()*sin(id*7.2)+x0);
        coordinate[i].setY(newcoord.y()+y0);
        coordinate[i].setZ(-newcoord.x()*sin(id*7.2)+newcoord.z()*cos(id*7.2)+z0);
    }
}


