#include "lidar/camera.h"
#include <math.h>

Camera::Camera():
    _theta(0.0f),
    _phi(0.0f),
    _d(1.0f),
    _roll(0.0f),
    _img_w(800),
    _img_h(600),
    _vfov(PI/4.0f),
    _far(1000.0f),
    _near(0.1f),
    _panRate(2.0f / 300),  // 2.0 per 300 pixels
    _zoomRate(0.8f),
    _rotateRate(PI / 2 / 256),  // PI/2 per 256 pixels
    _projection_mode(PERSPECTIVE)
{
    initLookAtPoint(0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f);
    save();
}

Camera::Camera(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax):
    _theta(0.0f),
    _phi(0.0f),
    _d(1.0f),
    _roll(0.0f),
    _img_w(800),
    _img_h(600),
    _vfov(PI/4.0f),
    _far(1000.0f),
    _near(0.1f),
    _panRate(2.0f / 300),  // 2.0 per 300 pixels
    _zoomRate(0.8f),
    _rotateRate(PI / 2 / 256),  // PI/2 per 256 pixels
    _projection_mode(PERSPECTIVE)
{
    initLookAtPoint(xmin, xmax,
                    ymin, ymax,
                    zmin, zmax);
    save();
    calCameraPosition();
}

void Camera::initLookAtPoint(float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax,
                             float altitude, float azimuth, float roll)
{

    // set m_look_at_point to center bottom w.r.t world coordinates
    _look_at_point.setX((xmin + xmax) / 2.0f);
    _look_at_point.setY((ymin + ymax) / 2.0f);
    _look_at_point.setZ((zmin + zmax) / 2.0f);
    _theta = altitude;
    _phi = azimuth;
    _roll = roll;

    // init camera position, spherical coordinates w.r.t to m_look_at_point
    // set m_d to length of widest span
    _d = (std::max)(xmax - xmin, (std::max)(ymax - ymin, zmax - zmin));
    calCameraPosition();
}

void Camera::save()
{
    _saved_look_at_point = _look_at_point;
    _saved_theta = _theta;
    _saved_phi = _phi;
    _saved_d = _d;
    _saved_roll = _roll;
}

void Camera::restore()
{
    _theta = _saved_theta;
    _phi = _saved_phi;
    _d = _saved_d;
    _roll = _saved_roll;
    _look_at_point = _saved_look_at_point;
    calCameraPosition();
}

void Camera::calCameraPosition(){
    // euler roll = 0.0f here
    QVector3D up_init = QVector3D(-sinf(_theta) * cosf(_phi),cosf(_theta),-sinf(_theta) * sinf(_phi));
    QVector3D right_init = QVector3D(sinf(_phi),0.0f,-cosf(_phi));
    QVector3D left_init = QVector3D(-sinf(_phi),0.0f,cosf(_phi));
    _view_vector = QVector3D(-cosf(_theta) * cosf(_phi),-sinf(_theta),-cosf(_theta) * sinf(_phi));
    // apply euler roll
    QQuaternion rotation = QQuaternion::fromAxisAndAngle(_view_vector, _roll*180.0f/3.1415926f);

    _up_vector = rotation * up_init;
    _right_vector = rotation * right_init;
    _left_vector = rotation * left_init;
}
// =============================
// accessor
// =============================
QVector3D Camera::getUpVector() const {
    return _up_vector;
}

QVector3D Camera::getRightVector() const{
    return _right_vector;
}

QVector3D Camera::getLeftVector() const
{
    return _left_vector;
}

QVector3D Camera::getViewVector() const {
    return _view_vector;
}

QVector3D Camera::getCameraPosition() const
{
    return getLookAtPosition() - _d * getViewVector();
}

QMatrix4x4 Camera::getWorld2CameraMatrix() const {
    QMatrix4x4 world2camera;
    world2camera.setToIdentity();
    world2camera.lookAt(getCameraPosition(), getLookAtPosition(), getUpVector());
    return world2camera;
}

const QMatrix4x4& Camera::getIntrinsicMatrix()
{
    _intrinsic_matrix.setToIdentity();
    if (_projection_mode == PERSPECTIVE){
        _intrinsic_matrix.perspective(_vfov/PI*180.0f, getAspectRatio(), _near, _far);
        return _intrinsic_matrix;
    }else{  // ORTHOGRAPHIC
        float t = _d * tanf(0.5f * 60.0f/180.0f*PI);
        float r = getAspectRatio() * t;
        _intrinsic_matrix.ortho(-r, r, -t, t, 0.8f * _near, 1.2f * _far);
        return _intrinsic_matrix;
    }
}

void Camera::setRotation(float theta_rad, float phi_rad){
    _theta = theta_rad;
    _phi = phi_rad;
    calCameraPosition();
}

// =============================
// camera transformation based on mouse movement (dx,dy)
// =============================
void Camera::rotate(float dx, float dy)
{
    _phi -= _rotateRate * dx;
    _theta += _rotateRate * dy;
    calCameraPosition();
}

void Camera::pan(float dx, float dy)
{
    _look_at_point = _look_at_point + _panRate * (getRightVector() * dx * (-1.0f) + getUpVector() * dy);
}

void Camera::zoom(float dx)
{
    _d = (std::max)(0.1f, _d * powf(_zoomRate, dx));
}

void Camera::rotate(QVector2D delta)
{
    // delta in screen space pixel scale
    if (delta.x() == 0.0f && delta.y() == 0.0f)
        return;
    if (_view_axis != ARBITRARY_AXIS)
        _view_axis = ARBITRARY_AXIS;
    rotate(delta.x(), delta.y());
    calCameraPosition();
}

void Camera::pan(QVector2D delta)
{
    _look_at_point = _look_at_point + _panRate * (getRightVector() * delta.x() * (-1.0f) + getUpVector() * delta.y());
}
