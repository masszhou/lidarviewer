#ifndef DUMMYBACKGROUND_H
#define DUMMYBACKGROUND_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector>

#include "vertex.h"

class DummyCanvas
{
public:
    DummyCanvas();
    DummyCanvas(float r, float g, float b);
    DummyCanvas(const QVector3D color);
    ~DummyCanvas();

    void initialze();
    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);

    void tearDown();
protected:
    void setColor(float r, float g, float b);

private:
    // shader
    QOpenGLBuffer _vbo;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram _program;

    QVector<Vertex> _model_vertexes;
};

inline DummyCanvas::DummyCanvas(){ setColor(0.1f, 0.1f, 0.15f); }
inline DummyCanvas::DummyCanvas(float r, float g, float b){ setColor(r, g, b); }
inline DummyCanvas::DummyCanvas(const QVector3D color){ setColor(color.x(), color.y(), color.z()); }

#endif // DUMMYBACKGROUND_H
