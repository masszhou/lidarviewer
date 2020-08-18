#include "camera_frustum.h"
#include <math.h>

CameraFrustum::CameraFrustum()
{

}

void CameraFrustum::xyz2ruv(float (&ruv)[3], const float (&xyz)[3]) const
{
    // note: handles case where xyz and ruv refer to same vector
    float xyz_[3];
    for (int dim = 0; dim < 3; dim++) {
        xyz_[dim] = xyz[dim] - _eye[dim];
    }
    ruv[0] = ruv[1] = ruv[2] = 0.0f;
    for (int k = 0; k < 3; k++) ruv[0] += _right[k] * xyz_[k];
    for (int k = 0; k < 3; k++) ruv[1] += _up[k] * xyz_[k];
    for (int k = 0; k < 3; k++) ruv[2] += _view[k] * xyz_[k];
}

void CameraFrustum::setImagePlane(float vfov, float aspect_ratio)
{
    // vfov in radians
    _image_t = tanf(0.5f * vfov);
    _image_r = aspect_ratio * _image_t;
}


void CameraFrustum::setupCameraFrustum(const Camera &camera, const float z_near, const float vfov, const float aspect_ratio)
{
    _eye = camera.getCameraPosition();
    _right = camera.getRightVector();
    _up = camera.getUpVector();
    _view = camera.getViewVector();
    setImagePlane(vfov, aspect_ratio);
    _z_near = z_near;
}

void CameraFrustum::setupOrthoCamera(const Camera &camera, const float vfov, const float aspect_ratio)
{
    _eye = camera.getCameraPosition();
    _right = camera.getRightVector();
    _up = camera.getUpVector();
    _view = camera.getViewVector();
    setImagePlane(vfov, aspect_ratio);
    _image_r *= camera.getCameraDistance();
    _image_t *= camera.getCameraDistance();
    _z_near = std::numeric_limits<float>::max();
}
