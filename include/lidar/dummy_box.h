#ifndef DUMMYBOX_H
#define DUMMYBOX_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "vertex.h"

class DummyBox
{
public:
    DummyBox();
    DummyBox(float x, float y, float z, float w, float h, float l);
    ~DummyBox();

    void initialze();
    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);

    void tearDown();
protected:
    void generateBox(QVector3D position, QVector3D dims);

private:
    // shader
    QOpenGLBuffer _vbo;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram _program;
    int u_model_to_world;
    int u_world_to_camera;
    int u_camera_to_view;

    // member
    QVector<Vertex> _model_vertexes;
    QVector3D _position;  // x, y, z
    QVector3D _dims;  // w, h, l

};



#endif // DUMMYBOX_H
