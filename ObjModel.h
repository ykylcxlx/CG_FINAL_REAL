#include <QVector>
#include <QString>
#include <QImage>
#include <QOpenGLTexture>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <GL/gl.h>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;


class Light {
public:
    QVector<QVector3D> direction;

};

class ObjModel {
public:
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    QVector<QVector3D> normals;
    QVector<GLuint> indices;
    QOpenGLTexture* texture1;
    QOpenGLTexture* texture2;
    QOpenGLTexture* texture3;

    ObjModel() : texture1(nullptr) { texture2 = nullptr;  texture3 = nullptr; }

    bool loadFromFile(const QString& filePath, const QString& texturePath, int id);
    void draw1(QVector3D explosion_center, QVector3D color, QVector3D* cameraLookat);
    void draw2(QVector3D explosion_center, QVector3D color, QVector3D* cameraLookat);
    void draw3(QVector3D explosion_center, QVector3D color, QVector3D* cameraLookat);
    QVector3D CalculateDirLight(QVector3D normal, QVector3D viewDir, QVector3D materialColor, GLfloat specularExponent);
};