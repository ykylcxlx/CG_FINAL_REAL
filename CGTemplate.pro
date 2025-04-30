QT += core gui opengl
QT += openglwidgets  
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += console qt c++11

DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += "./include/"


LIBS += \
	Glu32.lib \
	OpenGL32.lib\

# 添加 GLUT 库文件的路径
LIBS += -L$$PWD/include/GL -lglut32

# 链接 GLUT 库文件（不需要 .lib 后缀）
# LIBS += -lglut32

SOURCES += \
    $$files(*.cpp) \

HEADERS += \
    $$files(*.h) 
