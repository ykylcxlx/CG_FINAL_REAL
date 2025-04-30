#include <GL/glew.h>
#include "ObjModel.h"
#include <qvector3d>
#include <qvector2d>
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
#include <iostream>

using namespace std;
bool ObjModel::loadFromFile(const QString& filePath, const QString& texturePath, int id)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open .obj file";
        return false;
    }

    QTextStream stream(&file);
    QString line;

    // 解析文件内容
    while (!stream.atEnd()) {
        line = stream.readLine().trimmed();

        if (line.startsWith("v ")) {
            // 解析顶点坐标
            QStringList parts = line.split(" ");
            float x = parts[1].toFloat();
            float y = parts[2].toFloat();
            float z = parts[3].toFloat();
            if (id == 3) //帆船，先缩小模型后移动位置
            {
                x /= 20; x += 10; 
                y /= 20; y -= 10;
                z /= 20;
            }
            if (id == 2)//月亮，放大2倍并移动位置
            {
                x *= 5;
                x -= 20;
                y *= 5;
                y += 20;
                z *= 5;
            }
            vertices.append(QVector3D(x, y, z));
        }
        else if (line.startsWith("vt ")) {
            // 解析纹理坐标
            QStringList parts = line.split(" ");
            float u = parts[1].toFloat();
            float v = parts[2].toFloat();
            texCoords.append(QVector2D(u, v));
        }
        else if (line.startsWith("vn ")) {
            // 解析法向量
            QStringList parts = line.split(" ");
            float nx = parts[1].toFloat();
            float ny = parts[2].toFloat();
            float nz = parts[3].toFloat();
            normals.append(QVector3D(nx, ny, nz));
        }
        else if (line.startsWith("f ")) {
            // 解析面
            QStringList parts = line.split(" ");
            for (int i = 1; i < parts.size(); ++i) {
                QStringList vertexParts = parts[i].split("/");
                indices.append(vertexParts[0].toInt() - 1);  // 顶点索引
                if (vertexParts.size() > 1)
                {
                    indices.append(vertexParts[1].toInt() - 1);  // 纹理索引
                    if (vertexParts.size() > 2) 
                    indices.append(vertexParts[2].toInt() - 1);  // 法线索引
                }
            }
        }
    }

    file.close();

    // 加载纹理
    if (!texturePath.isEmpty()) {
        if(id == 1)
        texture1 = new QOpenGLTexture(QImage(texturePath).mirrored());
        else if (id ==2)
        texture2 = new QOpenGLTexture(QImage(texturePath));
        else if (id == 3)
        texture3 = new QOpenGLTexture(QImage(texturePath).mirrored());
    }

    return true;
}

QVector3D ObjModel::CalculateDirLight(QVector3D normal, QVector3D viewDir, QVector3D materialColor, GLfloat specularExponent) {
    // 平行光的方向
    QVector3D dirLightDir = QVector3D(-100.0f, 100.0f, 100.0f); // 假设平行光从照射
    dirLightDir.normalize();

    // 平行光的颜色和强度
    QVector3D dirLightColor = QVector3D(1.0f, 1.0f, 1.0f); // 平行光的颜色
    QVector3D dirLightAmbient = QVector3D(0.2f, 0.2f, 0.2f); // 平行光的环境光成分

    // 计算平行光照的漫反射部分
    QVector3D dirDiffuseComponent = QVector3D::dotProduct(normal, dirLightDir) * dirLightColor;
    dirDiffuseComponent = QVector3D(std::max(dirDiffuseComponent.x(), 0.0f),
        std::max(dirDiffuseComponent.y(), 0.0f),
        std::max(dirDiffuseComponent.z(), 0.0f));

    // 平行光的镜面反射部分
    QVector3D dirHalfDir = dirLightDir + viewDir;
    dirHalfDir.normalize();
    QVector3D dirSpecularComponent = std::pow(std::max(QVector3D::dotProduct(normal, dirHalfDir), 0.0f), specularExponent) * dirLightColor;

    // 平行光的总光照
    QVector3D dirTotalLight = dirLightAmbient + dirDiffuseComponent + dirSpecularComponent;

    return dirTotalLight;
}




void ObjModel::draw2(QVector3D explosion_center, QVector3D color, QVector3D* cameraLookat)
{
    if (texture2) {
        texture2->bind();  // 绑定纹理
    }
    glEnable(GL_LIGHTING); // 启用光照
    glEnable(GL_LIGHT0);   // 启用光源0
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    // 设置光源的位置
    GLfloat lightPos[] = { explosion_center.x(), explosion_center.y(), explosion_center.z(), 0.0f };  // 位置（点光源）

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_TEXTURE_2D); // 启用纹理映射
    //glBegin(GL_TRIANGLES);
    glBegin(GL_QUADS);

    GLfloat specularExponent = 64.0f;// 镜面指数（越高，高光越窄）
    //QVector3D ambientComponent = QVector3D(0.1, 0.1, 0.1);//环境光
    QVector3D viewDir = -(*cameraLookat); //视角方向，到底是负还是正？

    viewDir.normalize();
    QVector3D materialColor(1.0f, 1.0f, 1.0f); // 材质颜色
    color = color * 20;//提高烟花中心光照强度
    QVector3D ambientComponent = QVector3D(0.4f, 0.4f, 0.4f); // 平行光的环境光成分
    GLfloat ambientComponent1[] = { 0.2f,0.2f,0.2f,1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientComponent1);
    for (int i = 0; i < indices.size(); i += 3) {
        int vertexIndex = indices[i];
        int texCoordIndex = indices[i + 1];
        int normalIndex = indices[i + 2];

        QVector3D normal = normals[normalIndex];
        QVector3D vertex = vertices[vertexIndex];
        //注意，由于camera_position定义时是以（0,0,0）旋转的，所以实际上canmera_position就是cameralookat，云皓如果改了逻辑要重新看

        // 计算光照（Blinn-PhongShading模型）
        //vec3 parallel = CalcDirLight()
        //片元指向光源方向
        QVector3D lightDir = QVector3D(lightPos[0] - vertices[vertexIndex].x(), lightPos[1] - vertices[vertexIndex].y(), lightPos[2] - vertices[vertexIndex].z());  // 光源方向（假设在(1,1,1)位置）
        lightDir.normalize();

        //第一步 计算平行光照
        //QVector3D dirLightColor = CalculateDirLight(normal, viewDir, materialColor, specularExponent);

        // 计算法线和光源方向的点积，得到漫反射光强
        GLfloat diffuse = std::max(QVector3D::dotProduct(normal, lightDir), 0.0f);
        QVector3D diffuseComponent = diffuse * color * materialColor;
        GLfloat diff[] = { diffuseComponent.x(),diffuseComponent.y(),diffuseComponent.z(),1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);  // 漫反射光
        //计算镜面反射部分
        QVector3D halfDir = lightDir + viewDir;//半角向量
        halfDir.normalize();
        GLfloat specular = std::pow(std::max(QVector3D::dotProduct(normal, halfDir), 0.0f), specularExponent);
        QVector3D specularComponent = specular * color * materialColor;
        GLfloat spec[] = { specularComponent.x(),specularComponent.y(),specularComponent.z(),1.0f };
        glLightfv(GL_LIGHT0, GL_SPECULAR, diff);
        // 最终光照强度，累加环境光、漫反射和镜面反射
        QVector3D totalLight = diffuseComponent + specularComponent + ambientComponent;

        // 计算纹理坐标
        glTexCoord2f(texCoords[texCoordIndex].x(), texCoords[texCoordIndex].y());
        // 计算最终顶点颜色（简单的光照颜色：漫反射*材质颜色）
        GLfloat finalColor[] = { totalLight.x(), totalLight.y(), totalLight.z(), 1.0f };
        glColor3fv(finalColor); // 设置颜色（使用光照计算结果）

        // 绘制顶点
        glVertex3f(vertex.x(), vertex.y(), vertex.z());
    }

    glEnd();

    glDisable(GL_TEXTURE_2D); // 关闭纹理映射
    glDisable(GL_LIGHTING);   // 禁用光照
    if (texture2) {
        texture2->release();  // 释放纹理
    }
}
//专门用来画房子的
void ObjModel::draw3(QVector3D explosion_center, QVector3D color, QVector3D* cameraLookat)
{
    if (texture3) {
        texture3->bind();  // 绑定纹理
    }
    glEnable(GL_LIGHTING); // 启用光照
    glEnable(GL_LIGHT0);   // 启用光源0
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    // 设置光源的位置
    GLfloat lightPos[] = { explosion_center.x(), explosion_center.y(), explosion_center.z(), 0.0f };  // 位置（点光源）

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_TEXTURE_2D); // 启用纹理映射
    //glBegin(GL_TRIANGLES);
    glBegin(GL_QUADS);

    GLfloat specularExponent = 38.0f;// 镜面指数（越高，高光越窄）
    //QVector3D ambientComponent = QVector3D(0.1, 0.1, 0.1);//环境光
    QVector3D viewDir = -(*cameraLookat); //视角方向，到底是负还是正？

    viewDir.normalize();
    QVector3D materialColor(1.0f, 1.0f, 1.0f); // 材质颜色
    color = color*2 ;//提高烟花中心光照强度
    QVector3D ambientComponent = QVector3D(0.4f, 0.4f, 0.4f); // 平行光的环境光成分
    GLfloat ambientComponent1[] = { 0.4f,0.4f,0.4f,1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientComponent1);
    for (int i = 0; i < indices.size(); i += 3) {
        int vertexIndex = indices[i];
        int texCoordIndex = indices[i + 1];
        int normalIndex = indices[i + 2];

        QVector3D normal = normals[normalIndex];
        QVector3D vertex = vertices[vertexIndex];
        //注意，由于camera_position定义时是以（0,0,0）旋转的，所以实际上canmera_position就是cameralookat，云皓如果改了逻辑要重新看

        // 计算光照（Blinn-PhongShading模型）
        //vec3 parallel = CalcDirLight()
        //片元指向光源方向
        QVector3D lightDir = QVector3D(lightPos[0] - vertices[vertexIndex].x(), lightPos[1] - vertices[vertexIndex].y(), lightPos[2] - vertices[vertexIndex].z());  // 光源方向（假设在(1,1,1)位置）
        lightDir.normalize();

        //第一步 计算平行光照
        //QVector3D dirLightColor = CalculateDirLight(normal, viewDir, materialColor, specularExponent);

        // 计算法线和光源方向的点积，得到漫反射光强
        GLfloat diffuse = std::max(QVector3D::dotProduct(normal, lightDir), 0.0f);
        QVector3D diffuseComponent = diffuse * color * materialColor;
        GLfloat diff[] = { diffuseComponent.x(),diffuseComponent.y(),diffuseComponent.z(),1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);  // 漫反射光
        //计算镜面反射部分
        QVector3D halfDir = lightDir + viewDir;//半角向量
        halfDir.normalize();
        GLfloat specular = std::pow(std::max(QVector3D::dotProduct(normal, halfDir), 0.0f), specularExponent);
        QVector3D specularComponent = specular * color * materialColor;
        GLfloat spec[] = { specularComponent.x(),specularComponent.y(),specularComponent.z(),1.0f };
        glLightfv(GL_LIGHT0, GL_SPECULAR, diff);
        // 最终光照强度，累加环境光、漫反射和镜面反射
        QVector3D totalLight = diffuseComponent + specularComponent + ambientComponent;

        // 计算纹理坐标
        glTexCoord2f(texCoords[texCoordIndex].x(), texCoords[texCoordIndex].y());
        // 计算最终顶点颜色（简单的光照颜色：漫反射*材质颜色）
        GLfloat finalColor[] = { totalLight.x(), totalLight.y(), totalLight.z(), 1.0f };
        glColor3fv(finalColor); // 设置颜色（使用光照计算结果）

        // 绘制顶点
        glVertex3f(vertex.x(), vertex.y(), vertex.z());
    }

    glEnd();

    glDisable(GL_TEXTURE_2D); // 关闭纹理映射
    glDisable(GL_LIGHTING);   // 禁用光照
    if (texture3) {
        texture3->release();  // 释放纹理
    }
}

