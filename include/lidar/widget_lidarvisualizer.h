// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_TESTWIDGET_H
#define LIDARVIEWER_TESTWIDGET_H
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QWheelEvent>
#include <QSharedPointer>
#include <QPointer>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>

#include "camera.h"
#include "transform3d.h"
#include "dummy_box.h"
#include "dummy_boxes.h"
#include "dummy_car.h"
#include "dummy_canvas.h"
#include "dummy_axis.h"
//#include "dummy_floorgrid.h"
#include "selection_box.h"
#include "point_cloud.h"

#include <memory>


class LidarVisualizerWidget: public QOpenGLWidget
{
    Q_OBJECT
public:
    LidarVisualizerWidget(QWidget *parent = nullptr);

    // helper function
    QPointF normCoord(QPointF p);
    void initScene();
    void initSocketConnection();

protected slots:
    void update();
    void reply();

protected:
    void initializeGL() override;
    void resizeGL(int w,int h) override;
    void paintGL() override;

protected:
    virtual void keyPressEvent(QKeyEvent* ev) override;
    virtual void wheelEvent(QWheelEvent* ev) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* ev) override;
    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void mouseMoveEvent(QMouseEvent* ev) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev) override;

private:
    Transform3D _transform;
    Transform3D _transform_ego_car;
    Transform3D _transform_velo_to_gl;
    // transfor from kitti velodyne coord to opengl coord
    //        z x      y
    //        |/       |
    //   y -- v    =>  c -- x
    //                /
    //               z
    Camera _camera;

    int _screenshot_counter;

    // Non-QObject
    QSharedPointer<DummyBox> _dummy_box;
    QSharedPointer<DummyBoxes> _dummy_boxes;
    QSharedPointer<DummyAxis> _dummy_axis;

    QSharedPointer<DummyCar> _dummy_car;
    QSharedPointer<DummyCanvas> _dummy_background;
//    QSharedPointer<DummyFloorGrid> _dummy_floorgrid;
    QSharedPointer<SelectionBox> _selection_box;
    QSharedPointer<DummyBox> _car_bbox;
    Transform3D _car_bbox_trans;
    QSharedPointer<PointCloud> _point_cloud;

    // interaction
    QPointF _press_pos;

    // communication
    // QObject
    QPointer<QTcpServer> _server;
    QPointer<QTcpSocket> _socket_waiting_on_enter_key;
    QList<QPointer<QTcpSocket> >  _sockets;

};

#endif // LIDARVIEWER_TESTWIDGET_H
