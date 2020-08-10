// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_CAMERAFRUSTUM_H
#define LIDARVIEWER_CAMERAFRUSTUM_H
#include <QVector3D>
#include "camera.h"

class CameraFrustum
{
public:
    CameraFrustum();

    void xyz2ruv(float (&ruv)[3], const float (&xyz)[3]) const;
    void setImagePlane(float vfov, float aspect_ratio);
    void setupCameraFrustum(const Camera& camera, const float z_near, const float vfov, const float aspect_ratio);
    void setupOrthoCamera(const Camera& camera, const float vfov, const float aspect_ratio);

    float getImage_r() const;
    float getImage_t() const;
    float getZNear() const;
    QVector3D getCameraPostionVector() const;
    QVector3D getCameraRightVector() const;
    QVector3D getCameraUpVector() const;
    QVector3D getCameraViewVector() const;

private:
    QVector3D _eye;
    QVector3D _right;
    QVector3D _up;
    QVector3D _view;
    float _image_r;
    float _image_t;
    float _z_near;  // z_near < 0
};

inline float CameraFrustum::getImage_r() const { return _image_r; }
inline float CameraFrustum::getImage_t() const { return _image_t; }
inline float CameraFrustum::getZNear() const { return _z_near; }

inline QVector3D CameraFrustum::getCameraPostionVector() const { return _eye; }
inline QVector3D CameraFrustum::getCameraRightVector() const { return _right; }
inline QVector3D CameraFrustum::getCameraUpVector() const { return _up; }
inline QVector3D CameraFrustum::getCameraViewVector() const { return _view; }

#endif // LIDARVIEWER_CAMERAFRUSTUM_H
