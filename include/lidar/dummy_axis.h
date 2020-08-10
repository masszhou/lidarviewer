// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_DUMMY_AXIS_H
#define LIDARVIEWER_DUMMY_AXIS_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "vertex.h"

class dummy_axis
{
public:
    dummy_axis();
    dummy_axis(float cx, float cy, float cz, QVector3D x_basis, QVector3D y_basis, QVector3D z_basis);

    void initialze();
    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);

    void tearDown();

private:
    // shader
    QOpenGLBuffer _vbo;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram _program;
    int u_model_to_world;
    int u_world_to_camera;
    int u_camera_to_view;

    QVector<Vertex> _vertexes;

    QVector3D _position;  // x, y, z
    QVector3D _dims;  // w, h, l
};

#endif // LIDARVIEWER_DUMMY_AXIS_H
