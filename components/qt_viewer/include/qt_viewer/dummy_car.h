// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_DUMMYCAR_H
#define LIDARVIEWER_DUMMYCAR_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector>

#include "vertex.h"

class DummyCar
{
public:
    DummyCar();
    ~DummyCar();

    void loadModel(const QString& passat_fileName, const QString& wheel_fileName, QVector<float> &vPoints,QVector<float> &tPoints,QVector<float> &nPoints);
    void initialze();
    void initializeShader(const QString& vert_file, const QString& frag_file,
                          QOpenGLShaderProgram& program, QOpenGLBuffer& vbo, QOpenGLVertexArrayObject& vao,
                          const QVector<Vertex>& vertexes);

    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);

    void tearDown();

private:
    // shader
    QOpenGLBuffer _vbo_body;
    QOpenGLVertexArrayObject _vao_body;
    QOpenGLShaderProgram _program_body;
    int u_model_to_world_body;
    int u_world_to_camera_body;
    int u_camera_to_view_body;
    QVector<Vertex> _vertexes_body;
    QVector<float> _vertex_pts, _texture_pts, _normal_pts;

    QOpenGLBuffer _vbo_wheel;
    QOpenGLVertexArrayObject _vao_wheel;
    QOpenGLShaderProgram _program_wheel;
    int u_model_to_world_wheel;
    int u_world_to_camera_wheel;
    int u_camera_to_view_wheel;
    QVector<Vertex> _vertexes_wheel;

    float _point_size;

};


#endif // LIDARVIEWER_DUMMYCAR_H
