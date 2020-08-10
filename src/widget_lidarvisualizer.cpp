#include <iostream>
#include <memory>
#include <QDate>

#include "lidar/widget_lidarvisualizer.h"

#include "lidar/comm_funcs.h"


LidarVisualizerWidget::LidarVisualizerWidget(QWidget *parent):
    _screenshot_counter(0),
    _dummy_box(nullptr)
{
    initScene();
}


void LidarVisualizerWidget::initScene() {
    // --------------------------------------------------------------------------
    // opengl initialization
    // --------------------------------------------------------------------------
    _transform.setTranslation(0.0f, 0.0f, 0.0f);  // set box at origin

    _transform_ego_car.setTranslation(0.0f, 0.0f, -1.7f);
    _transform_ego_car.rotate(90.0f, QVector3D(1.0f, 0.0f, 0.0f));  // rotate around x axis for 90 degrees

    // transfor from velodyne coord to opengl coord
    //        z x      y
    //        |/       |
    //   y -- v    =>  c -- x
    //                /
    //               z
    _transform_velo_to_gl.setTranslation(0.0f, 0.0f, 0.0f);  // no offset for label boxes

    _camera.initLookAtPoint(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 3.14159f/2.0f, 0.0f);

    setFocusPolicy(Qt::StrongFocus);

    _dummy_box = QSharedPointer<DummyBox>(new DummyBox(-10.774f, -4.827f,  -1.014f, 1.556f, 3.858f, 1.402f));  // x,y,z,w,h,l in kitti velodyne coordinates

    QVector<float> boxes = {-10.774345, -4.8273034, -1.0148145, 1.5563552, 3.8583605, 1.4025569, -3.1160157,
                            13.7103815, -0.9327341, -0.9246998, 1.748823, 4.018608, 1.5223817, -1.5574207};
    _dummy_boxes = QSharedPointer<DummyBoxes>(new DummyBoxes);

    _dummy_car = QSharedPointer<DummyCar>(new DummyCar);
    _selection_box = QSharedPointer<SelectionBox>(new SelectionBox);
    _dummy_background = QSharedPointer<DummyCanvas>(new DummyCanvas);
    //    _dummy_floorgrid = QSharedPointer<DummyFloorGrid>(new DummyFloorGrid);

    _car_bbox = QSharedPointer<DummyBox>(new DummyBox(0.0f, 0.0f, 0.0f, 4.8f, 1.6f, 2.1f));  // x,y,z,w,h,l
    _car_bbox_trans.setTranslation(0.0f, 0.7f, 0.0f);

    _point_cloud = QSharedPointer<PointCloud>(new PointCloud);

    _dummy_axis = QSharedPointer<DummyAxis>(new DummyAxis());
}

void LidarVisualizerWidget::initSocketConnection()
{
    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
    quint16 number = 50000;
    quint16 random_port = qrand() % number;

    // --------------------------------------------------------------------------
    // setup tcp server
    // --------------------------------------------------------------------------
    // set up TCP server for receiving commands from Python terminal (client)
    _server = QPointer<QTcpServer>(new QTcpServer());
    quint16 server_port = random_port;
    if (!_server->listen(QHostAddress::LocalHost, server_port)) {
        qDebug() << _server->errorString().toLocal8Bit().constData();
        exit(1);
    }
    connect(_server.data(), SIGNAL(newConnection()), this, SLOT(reply()));
    qDebug() << "[Viewer]: TCP server set up on port " << _server->serverPort();
}


QPointF LidarVisualizerWidget::normCoord(QPointF p)
{
    QVector2D v = QVector2D(p) * QVector2D(2.0f / width(), -2.0f / height()) + QVector2D(-1.0f, 1.0f);
    //QVector2D v = QVector2D(p) * QVector2D(1.0f / width(), 1.0f / height());
    return QPointF(double(v.x()), double(v.y()));
}

void LidarVisualizerWidget::update()
{
    QOpenGLWidget::update();
}

void LidarVisualizerWidget::reply()
{
    qDebug() << "[Viewer]: server got signal";
    QTcpSocket* clientConnection = _server->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

    // read first byte of incoming message
    char msgType;
    comm::receiveBytes(&msgType, 1, clientConnection);
    qDebug() << "[Viewer]: received message type " << static_cast<int>(msgType);

    // switch on message type
    switch (msgType) {
    case 1: {  // load points
        // receive point count (next 4 bytes)
        uint32_t numPoints;
        comm::receiveBytes(reinterpret_cast<char*>(&numPoints), sizeof(uint32_t), clientConnection);  // equivalent to comm::receiveBytes((char*)&numPoints, sizeof(qint32), clientConnection);
        qDebug() << "[Viewer]: expecting" << numPoints << "points";

        // receive position vectors
        // (next 3 x numPoints x sizeof(float) bytes)
        std::vector<float> positions(3 * numPoints);
        comm::receiveBytes(reinterpret_cast<char*>(&positions[0]), positions.size() * sizeof(float), clientConnection);
        qDebug() << "[Viewer]: received float data" << positions.size();

        _point_cloud->loadPoints(positions);
        break;
    }
    case 2: {  // clear points
        qDebug() << "clear points";
        //        _points->clearPoints();
        //        renderPoints();
        //        renderPointsFine();
        break;
    }
    case 3: {  // reset view to fit all
        qDebug() << "reset view to fit all";
        //        _camera = QtCamera(_points->getBox());
        //        _camera.setAspectRatio((float)width() / height());
        //        renderPoints();
        //        renderPointsFine();
        break;
    }
    case 4: {  // set viewer property
        qDebug() << "set viewer property";
        break;
    }
    case 5: {  // get viewer property
        qDebug() << "get viewer property";
        break;
    }
    case 6: {  // print screen
        qDebug() << "print screen";
        break;
    }
    case 7: {  // wait for enter
        qDebug() << "wait for enter";
        // save current connection socket and return
        _socket_waiting_on_enter_key = clientConnection;
        return;
    }
    case 8: {  // load camera path animation
        qDebug() << "load camera path animation";
        break;
    }
    case 9: {  // playback camera path animation
        qDebug() << "playback camera path animation";
        break;
    }
    case 10: {  // set per point attributes
        qDebug() << "set per point attributes";
        // receive point count (next 4 bytes)
        uint32_t msg_data_length;
        comm::receiveBytes(reinterpret_cast<char*>(&msg_data_length), sizeof(uint32_t), clientConnection);

        // receive color vectors
        // (next 3 x numPoints x sizeof(float) bytes)
        std::vector<float> colors(msg_data_length);
        comm::receiveBytes(reinterpret_cast<char*>(&colors[0]), colors.size() * sizeof(float), clientConnection);
        _point_cloud->loadColor(colors);
        break;
    }
    case 11: {
        qDebug() << "rec label boxes";
        // receive point count (next 4 bytes)
        uint32_t numBoxes;
        comm::receiveBytes(reinterpret_cast<char *>(&numBoxes), sizeof(uint32_t), clientConnection);
        qDebug() << "[Viewer]: expecting" << numBoxes << "boxes";

        // receive boxes
        if (numBoxes > 0){
            QVector<float> xyzwhlyaw(7 * numBoxes);
            comm::receiveBytes(reinterpret_cast<char*>(&xyzwhlyaw[0]), xyzwhlyaw.size() * sizeof(float), clientConnection);
            qDebug() << "[Viewer]: received float data" << xyzwhlyaw.size();

            if (!xyzwhlyaw.isEmpty())
                _dummy_boxes->loadBoxes(xyzwhlyaw);
        }

        break;
    }
    default:{  // unrecognized message type
        break;
        // do nothing
    }
    }
    clientConnection->write("1234");
    clientConnection->disconnectFromHost();
}

void LidarVisualizerWidget::initializeGL()
{
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    _dummy_background->initialze();
    //    _dummy_floorgrid->initialze();

    _dummy_box->initialze();
    _dummy_boxes->initialze();
    _dummy_car->initialze();
    _car_bbox->initialze();
    _selection_box->initialze();
    _point_cloud->initialze();
    _dummy_axis->initialze();
}

void LidarVisualizerWidget::resizeGL(int w, int h)
{
    _camera.setImgSize(w, h);
}

// void QOpenGLWindow::makeCurrent() is called automatically before invoking paintGL()
void LidarVisualizerWidget::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f,0.0f,0.0f,1.0f);

    _dummy_background->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform.getObject2WorldMatrix());
    _point_cloud->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform_velo_to_gl.getObject2WorldMatrix());
    _dummy_car->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform_ego_car.getObject2WorldMatrix());

//    _dummy_box->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform_velo_to_gl.getObject2WorldMatrix());
    _dummy_boxes->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform_velo_to_gl.getObject2WorldMatrix());
    //    _car_bbox->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _car_bbox_trans.getObject2WorldMatrix());
    _selection_box->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform.getObject2WorldMatrix());
    _dummy_axis->render(f, _camera.getIntrinsicMatrix(), _camera.getWorld2CameraMatrix(), _transform.getObject2WorldMatrix());
}

// =================================================
// interaction
// =================================================

void LidarVisualizerWidget::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_B){
        qDebug() << "debug";

        qDebug() << "eye: " << _camera.getCameraPosition();
        qDebug() << "right: " << _camera.getRightVector();
        qDebug() << "left: " << _camera.getLeftVector();
        qDebug() << "up: " << _camera.getUpVector();
        qDebug() << "view: " << _camera.getViewVector();
        qDebug() << "width: " << _camera.getImgWidth() << ", height: " << _camera.getImgHeight();
        qDebug() << "AspectRatio: " << _camera.getAspectRatio();

    }else if (ev->key() == Qt::Key_P) {
        if (_camera.getProjectionMode()==Camera::PERSPECTIVE)
            _camera.setProjectionMode(Camera::ORTHOGRAPHIC);
        else
            _camera.setProjectionMode(Camera::PERSPECTIVE);
    }else if (ev->key() == Qt::Key_1) {
        _camera.setRotation(0.0f, 0.0f); // look from x axis
    }else if (ev->key() == Qt::Key_2) {
        _camera.setRotation(PI/2.0f, 0.0f); // look from y axis
    }else if (ev->key() == Qt::Key_3) {
        _camera.setRotation(0.0f, PI/2.0f); // look from z axis
    }else if (ev->key() == Qt::Key_S){
        qDebug() << "save screen to image_" + QString::number(_screenshot_counter) + ".png";
        this->grab().save("image_" + QString::number(_screenshot_counter) + ".png");
        _screenshot_counter += 1;
    }else if (ev->key() == Qt::Key_R){
        _transform_velo_to_gl.setRotation(0.0f, 0.0f, 0.0f);
    }else if (ev->key() == Qt::Key_C){
        _transform_velo_to_gl.setTranslation(0.0f, 0.0f, 0.0f);  // no offset for label boxes
        _transform_velo_to_gl.rotate(90.0f, QVector3D(0.0f, 1.0f, 0.0f));  // rotate around y axis for 90 degrees
        _transform_velo_to_gl.rotate(90.0f, QVector3D(0.0f, 0.0f, 1.0f));  // rotate around z axis for 90 degrees
    }

}

void LidarVisualizerWidget::wheelEvent(QWheelEvent *ev)
{
    // note: angleDelta() is in units of 1/8 degree
    _camera.zoom(ev->angleDelta().y() / 120.0f);
    _camera.save();
}

void LidarVisualizerWidget::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
}

void LidarVisualizerWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        _press_pos = ev->windowPos();
        _camera.save();

        if (ev->modifiers() & Qt::ControlModifier) {
            if (ev->modifiers() & Qt::ShiftModifier)
                _selection_box->click(normCoord(_press_pos), SelectionBox::SUB);
            else
                _selection_box->click(normCoord(_press_pos), SelectionBox::ADD);
        }
    }
}

void LidarVisualizerWidget::mouseMoveEvent(QMouseEvent *ev)
{
    // note: +x right, +y down
    if (ev->buttons() & Qt::LeftButton) {

        if (_selection_box->isActive()) {
            _selection_box->drag(normCoord(ev->windowPos()));
        } else {
            _camera.restore();
            if (ev->modifiers() == Qt::ShiftModifier){
                _camera.pan(QVector2D(ev->windowPos() - _press_pos));
            }else if (ev->modifiers() == Qt::NoModifier){
                _camera.rotatePitchYaw(QVector2D(ev->windowPos() - _press_pos));
            }else if (ev->modifiers() == Qt::AltModifier){
                _camera.rotateRoll(QVector2D(ev->windowPos() - _press_pos).x());
            }
        }
    }
}

void LidarVisualizerWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    QPointF releasePos = ev->windowPos();
    bool mouse_moved = releasePos != _press_pos;
    if (_selection_box->isActive()) {
        _selection_box->release();
    }else{
        if (mouse_moved) {
            _camera.save();
        }
    }
}
