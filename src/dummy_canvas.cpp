#include "lidar/dummy_canvas.h"


DummyCanvas::~DummyCanvas(){
    tearDown();
}

void DummyCanvas::initialze()
{
    _program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/canvas.vert");  // add Vertex shader
    _program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/canvas.frag");  // add Fragment shader
    _program.link();  // Link all of the loaded shaders together. here are Vertex shader and Fragment shader
    _program.bind();  // Bind the shader so that it is the current active shader.


    // Create Buffer (Do not release until VAO is created)
    _vbo.create();  // Create a buffer for dynamic allocation later.
    _vbo.bind();  // Bind the buffer so that it is the current active buffer.
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);  // Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw.
    _vbo.allocate(_model_vertexes.data(), static_cast<int>(sizeof(_model_vertexes[0])) * _model_vertexes.size());  // Allocate


    // Create Vertex Array Object
    _vao.create();
    _vao.bind();
    _program.enableAttributeArray(0);
    _program.enableAttributeArray(1);
    _program.setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    _program.setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

    // Release (unbind) all
    _vao.release();  // paired with bind()
    _vbo.release();
    _program.release();
}

void DummyCanvas::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    Q_UNUSED(pMatrix)
    Q_UNUSED(vMatrix)
    Q_UNUSED(mMatrix)

    _program.bind();
    {
        _vao.bind();
        f->glDrawArrays(GL_TRIANGLES, 0, _model_vertexes.size());
        _vao.release();
    }
    _program.release();
}

void DummyCanvas::tearDown()
{
    _vao.destroy();
    _vbo.destroy();
}

void DummyCanvas::setColor(float r, float g, float b)
{
    r = r < 1.0f ? r : 1.0f;
    r = r > 0.0f ? r : 0.0f;
    g = g < 1.0f ? g : 1.0f;
    g = g > 0.0f ? g : 0.0f;
    b = b < 1.0f ? b : 1.0f;
    b = b > 0.0f ? b : 0.0f;

    // triangle 1
    _model_vertexes <<Vertex( QVector3D(-1.0f,  -1.0f, 0.0f), QVector3D( r, g, b ));
    _model_vertexes <<Vertex( QVector3D( 1.0f,  -1.0f, 0.0f), QVector3D( r, g, b ));
    _model_vertexes <<Vertex( QVector3D( 1.0f,  1.0f, 0.0f), QVector3D( r, g, b ));
    // triangle 2
    _model_vertexes <<Vertex( QVector3D(-1.0f,  -1.0f, 0.0f), QVector3D( r, g, b ));
    _model_vertexes <<Vertex( QVector3D( 1.0f,   1.0f, 0.0f), QVector3D( r, g, b ));
    _model_vertexes <<Vertex( QVector3D(-1.0f,   1.0f, 0.0f), QVector3D( r, g, b ));
}
