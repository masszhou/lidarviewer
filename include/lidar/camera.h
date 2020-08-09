#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QVector2D>
#include <QQuaternion>
#include <QMatrix4x4>
#define PI 3.14159265359f

/**
 * @brief The Camera class, right hand coordinates
 */
class Camera
{
public:
    enum ProjectionMode {PERSPECTIVE = 0, ORTHOGRAPHIC = 1};
    enum ViewAxis {ARBITRARY_AXIS, X_AXIS, Y_AXIS, Z_AXIS};

    // =============================
    // constructor
    // =============================
    Camera();
    Camera(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

    // =============================
    // camera transformation based on mouse movement (dx,dy)
    // =============================
    void rotate(float dx, float dy);
    void pan(float dx, float dy);
    void zoom(float dx);

    void rotate(QVector2D delta);
    void pan(QVector2D delta);

    // =============================
    // accessor
    // =============================
    const QVector3D& getLookAtPosition() const;
    float getTheta() const;
    float getPhi() const;
    float getCameraDistance() const;

    QVector3D getUpVector() const;
    QVector3D getRightVector() const;
    QVector3D getLeftVector() const;
    QVector3D getViewVector() const;
    QVector3D getCameraPosition() const;
    QMatrix4x4 getWorld2CameraMatrix() const;
    const QMatrix4x4& getIntrinsicMatrix();

    int getImgWidth() const;
    int getImgHeight() const;
    float getAspectRatio() const;
    float getVerticalFOV() const;
    ProjectionMode getProjectionMode() const;

    void setImgSize(int w, int h);

    void setRotation(float theta_rad, float phi_rad);
    void setProjectionMode(ProjectionMode mode);

    // =============================
    // helper functions
    // =============================
    void initLookAtPoint(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
    void save();
    void restore();

private:
    // =============================
    // spherical coordinates w.r.t to m_look_at_point
    // key definition of camera pose
    // other states are derived from these properties
    // =============================
    float _theta;  // angle of elevation in [Radius], from axis-x to axis-y is positive, w.r.t to _look_at_point
    float _phi;    // azimuthal angle in [Radius], from axis-x to axis-z is positive, w.r.t to _look_at_point
    float _d;      // distance between camera and m_look_at_point, w.r.t to _look_at_point
    float _roll;   // euler angle, related to m_up_vector
    // buffer
    float _saved_theta;
    float _saved_phi;
    float _saved_d;
    float _saved_roll;
    QVector3D _saved_look_at_point;

    // =============================
    // cartesian coordinates
    // =============================
    QVector3D _position;  // x, y, z in world coordinates
    QVector3D _look_at_point;  // x, y, z in world coordinates

    // =============================
    // camera model
    // =============================
    QMatrix4x4 _intrinsic_matrix;
    QMatrix4x4 _world2camera_matrix;
    int _img_w;
    int _img_h;
    float _vfov;  // [rad] vertical angle of the frustum
    float _far;   // [meters]
    float _near;  // [meters]

    // =============================
    // camera movement control
    // =============================
    float _panRate;
    float _zoomRate;
    float _rotateRate;

    // =============================
    // state
    // =============================
    ProjectionMode _projection_mode;
    ViewAxis _view_axis;
};

// =============================
// accessor
// =============================
inline const QVector3D& Camera::getLookAtPosition() const { return _look_at_point; }

inline float Camera::getTheta() const{ return _theta; }
inline float Camera::getPhi() const{ return _phi; }
inline float Camera::getCameraDistance() const{ return _d; }

inline int Camera::getImgWidth() const{ return _img_w; }
inline int Camera::getImgHeight() const { return _img_h; }
inline float Camera::getAspectRatio() const { return static_cast<float>(_img_w)/_img_h; }
inline float Camera::getVerticalFOV() const { return _vfov;}

inline Camera::ProjectionMode Camera::getProjectionMode() const{ return _projection_mode; }
inline void Camera::setImgSize(int w, int h){
    _img_w = w;
    _img_h = h;
}

inline void Camera::setProjectionMode(Camera::ProjectionMode mode){ _projection_mode = mode; }


#endif // CAMERA_H
