#ifndef VIEWER_H
#define VIEWER_H
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QWheelEvent>
#include "transform3d.h"

#include "camera.h"


class QOpenGLShaderProgram;

class ViewerWidget : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ViewerWidget();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();  //This is called whenever the widget needs to be painted.

//    void loadObject();

protected slots:
    void teardownGL();
    void update();

protected:
    virtual void keyPressEvent(QKeyEvent* ev);
    virtual void wheelEvent(QWheelEvent* ev);
    virtual void mousePressEvent(QMouseEvent* ev);
    virtual void mouseMoveEvent(QMouseEvent* ev);
    virtual void mouseReleaseEvent(QMouseEvent* ev);

private:
    // OpenGL State Information
    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;

    // Shader Information
    int u_model_to_world;
    int u_world_to_camera;
    int u_camera_to_view;
    QMatrix4x4 m_projection;
    Transform3D m_transform;
    Camera m_camera;

    // interaction
    QPointF m_press_pos;

    // Private Helpers
    void printVersionInformation();

};
#endif // VIEWER_H
