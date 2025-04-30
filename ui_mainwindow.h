/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtGui/QKeyEvent> // 用于捕获键盘事件

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget* centralWidget;
    QPushButton* pushButton;
    QLabel* label;
    QStatusBar* statusBar;

    void setupUi(QMainWindow* MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1231, 921);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");

        // 创建控制按钮
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(49, 90, 121, 61));
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));

        // 创建显示标签
        label = new QLabel(centralWidget);
        label->setObjectName("label");
        label->setGeometry(QRect(0, 120, 1041, 521));

        // 设置主窗口的中央小部件
        MainWindow->setCentralWidget(centralWidget);

        // 创建状态栏
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    }

    void retranslateUi(QMainWindow* MainWindow)
    {
        MainWindow->setWindowTitle(QString::fromUtf8("Fireworks"));
        pushButton->setText(QString::fromUtf8("Start"));

        QString htmlText = QString::fromUtf8("<html><head/><body><p><span style=\" color:#ffffff;\">This is a </span><span style=\" font-weight:600; color:#ffffff;\">Fireworks</span><br><span style=\" color:#ffffff;\"> particle system</span></p>"
            "<p><span style = \" color:#ffffff;\">Please first press <span style=\" font-weight:600; color:#ffffff;\">S</span> <br> for 3 times.</span></p>"
            "<p><span style=\" color:#ffffff;\">You can control it <br> with the keyboard:</span></p>"
            "<p><span style=\" color:#ffffff;\">Press <span style=\" font-weight:600; color:#ffffff;\">W</span> key - Go Forward</span></p>"
            "<p><span style=\" color:#ffffff;\">Press <span style=\" font-weight:600; color:#ffffff;\">S</span> key - Go Backward</span></p>"
            "<p><span style=\" color:#ffffff;\">Press <span style=\" font-weight:600; color:#ffffff;\">A</span> key - Go Left</span></p>"
            "<p><span style=\" color:#ffffff;\">Press <span style=\" font-weight:600; color:#ffffff;\">D</span> key - Go Right</span></p>"
            "<p><span style=\" color:#ffffff;\">Left mouse button can<br>control camera rotation<br> direction</span></p>"
            "<p><span style=\" color:#ffffff;\">Press space bar<br>to show fireworks.</span></p></body></html>"
        );
        label->setText(htmlText);
    }

};

namespace Ui {
    class MainWindow : public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
