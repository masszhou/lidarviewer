#include <QApplication>
#include <QMainWindow>
#include <QPainter>
#include <QtDebug>
#include <vector>

//#include "window_viewer.h"
#include "lidar/widget_lidarvisualizer.h"


static void usage()
{
    qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
    exit(1);
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set OpenGL Version information
    // Note: This format must be set before show() is called.
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);

    // Set widget up
    LidarVisualizerWidget *widget = new LidarVisualizerWidget;
    widget->setFormat(format);
//    widget->initSocketConnection();

    // Set the window up
    QMainWindow window;
    window.setCentralWidget(widget);
    window.resize(QSize(800, 600));
    window.show();

    return app.exec();
}

