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
    initLookAtPoint(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
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
    initLookAtPoint(xmin, xmax, ymin, ymax, zmin, zmax);
    save();
}

void Camera::initLookAtPoint(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
    // set m_look_at_point to center bottom w.r.t world coordinates
    _look_at_point.setX((xmin + xmax) / 2.0f);
    _look_at_point.setY((ymin + ymax) / 2.0f);
    _look_at_point.setZ((zmin + zmax) / 2.0f);

    // init camera position, spherical coordinates w.r.t to m_look_at_point
    // set m_d to length of widest span
    _d = (std::max)(xmax - xmin, (std::max)(ymax - ymin, zmax - zmin));
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
}

// =============================
// accessor
// =============================
QVector3D Camera::getUpVector() const {
    // euler roll = 0.0f here
    return QVector3D(-sinf(_theta) * cosf(_phi),
                     cosf(_theta),
                     -sinf(_theta) * sinf(_phi));
}

QVector3D Camera::getRightVector() const{
    return QVector3D(sinf(_phi),
                     0.0f,
                     -cosf(_phi));
}

QVector3D Camera::getLeftVector() const
{
    return QVector3D(-sinf(_phi),
                     0.0f,
                     cosf(_phi));
}

QVector3D Camera::getViewVector() const {
    return QVector3D(-cosf(_theta) * cosf(_phi),
                     -sinf(_theta),
                     -cosf(_theta) * sinf(_phi));
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
}

// =============================
// camera transformation based on mouse movement (dx,dy)
// =============================
void Camera::rotate(float dx, float dy)
{
    _phi -= _rotateRate * dx;
    _theta += _rotateRate * dy;
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
}

void Camera::pan(QVector2D delta)
{
    _look_at_point = _look_at_point + _panRate * (getRightVector() * delta.x() * (-1.0f) + getUpVector() * delta.y());
}
