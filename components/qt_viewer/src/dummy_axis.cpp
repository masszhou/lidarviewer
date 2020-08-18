#include "dummy_axis.h"

DummyAxis::DummyAxis(float scale):
        u_model_to_world(0),
        u_world_to_camera(0),
        u_camera_to_view(0)
{
    QVector3D origin = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D x_basis = QVector3D(1.0f*scale, 0.0f, 0.0f);
    QVector3D y_basis = QVector3D(0.0f, 1.0f*scale, 0.0f);
    QVector3D z_basis = QVector3D(0.0f, 0.0f, 1.0f*scale);
    // o->x
    QVector3D r = QVector3D( 1.0f, 0.0f, 0.0f );
    _vertexes << Vertex( origin, r ) << Vertex( x_basis, r );
    // o->y
    QVector3D g = QVector3D( 0.0f, 1.0f, 0.0f );
    _vertexes << Vertex( origin, g ) << Vertex( y_basis, g );
    // o->x
    QVector3D b = QVector3D( 0.0f, 0.0f, 1.0f );
    _vertexes << Vertex( origin, b ) << Vertex( z_basis, b );

}

DummyAxis::DummyAxis(QVector3D origin, QVector3D x_basis, QVector3D y_basis, QVector3D z_basis):
    u_model_to_world(0),
    u_world_to_camera(0),
    u_camera_to_view(0)
{
    // o->x
    QVector3D r = QVector3D( 1.0f, 0.0f, 0.0f );
    _vertexes << Vertex( origin, r ) << Vertex( x_basis, r );
    // o->y
    QVector3D g = QVector3D( 0.0f, 1.0f, 0.0f );
    _vertexes << Vertex( origin, g ) << Vertex( y_basis, g );
    // o->x
    QVector3D b = QVector3D( 0.0f, 0.0f, 1.0f );
    _vertexes << Vertex( origin, b ) << Vertex( z_basis, b );

}

void DummyAxis::initialze()
{
    //_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vsCode.c_str());  // add Vertex shader
    _program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/box_3d.vert");  // add Vertex shader
    _program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/box_3d.frag");  // add Fragment shader
    _program.link();  // Link all of the loaded shaders together. here are Vertex shader and Fragment shader
    _program.bind();  // Bind the shader so that it is the current active shader.

    // Cache Uniform Locations
    u_model_to_world = _program.uniformLocation("modelToWorld");  // index
    u_world_to_camera = _program.uniformLocation("worldToCamera");  // index
    u_camera_to_view = _program.uniformLocation("cameraToView");  // index

    // Create Buffer (Do not release until VAO is created)
    _vbo.create();  // Create a buffer for dynamic allocation later.
    _vbo.bind();  // Bind the buffer so that it is the current active buffer.
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);  // Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw.
    _vbo.allocate(_vertexes.data(), static_cast<int>(sizeof(_vertexes[0])) * _vertexes.size());  // Allocate and upload

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

void DummyAxis::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    f->glEnable(GL_CULL_FACE);

    _program.bind();
    _program.setUniformValue(u_model_to_world, mMatrix);
    _program.setUniformValue(u_world_to_camera, vMatrix);
    _program.setUniformValue(u_camera_to_view, pMatrix);
    {
        _vao.bind();
        f->glDrawArrays(GL_LINES, 0, _vertexes.size());
        _vao.release();
    }
    _program.release();
}

void DummyAxis::tearDown()
{

}
