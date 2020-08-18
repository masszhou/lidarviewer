#include <QApplication>
#include <QMainWindow>
#include <QPainter>
#include <QtDebug>
#include <vector>
#include <iostream>

#include "widget_lidarvisualizer.h"


static void usage()
{
    qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
    exit(1);
}


int main(int argc, char *argv[])
{
    if (argc < 2){
        std::cout << "need socket port number, e.g. $./lidarviewer_exec 10086" << std::endl;
        return -1;
    }

    QApplication app(argc, argv);

    // Set OpenGL Version information
    // Note: This format must be set before show() is called.
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);

    // Set widget up
    auto *widget = new LidarVisualizerWidget(atoi(argv[1]));
    widget->setFormat(format);
//    widget->initSocketConnection();

    // Set the window up
    QMainWindow window;
    window.setCentralWidget(widget);
    window.resize(QSize(800, 600));
    window.show();

    return app.exec();
}

