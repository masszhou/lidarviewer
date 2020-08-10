// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_DUMMYBOXES_H
#define LIDARVIEWER_DUMMYBOXES_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "vertex.h"

class DummyBoxes
{
public:
    DummyBoxes();

    ~DummyBoxes();

    void initialze();
    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);
    void tearDown();

    void loadBoxes(const QVector<float>& xyzwhlyaw_list);

protected:
    void generateBox(float x, float y, float z, float w, float h, float l, float yaw);
    void copyVertexesToVBO();

private:
    // shader
    QOpenGLBuffer _vbo;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram _program;
    int u_model_to_world;
    int u_world_to_camera;
    int u_camera_to_view;

    // member
    QVector<Vertex> _vertexes;
    bool _ready;
    uint32_t _max_n_boxes;

};

#endif // LIDARVIEWER_DUMMYBOXES_H
