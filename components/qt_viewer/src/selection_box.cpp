#include "selection_box.h"

SelectionBox::SelectionBox():
    _select_mode(NONE),
    _box_min(0.0, 0.0),
    _box_max(0.0, 0.0)
{
    _model_vertexes <<Vertex( QVector3D(0.0f,  0.0f, 0.0f), QVector3D( 0.0f, 1.0f, 1.0f ));
    _model_vertexes <<Vertex( QVector3D(1.0f,  0.0f, 0.0f), QVector3D( 0.0f, 1.0f, 1.0f ));
    _model_vertexes <<Vertex( QVector3D(1.0f,  1.0f, 0.0f), QVector3D( 0.0f, 1.0f, 1.0f ));
    _model_vertexes <<Vertex( QVector3D(0.0f,  1.0f, 0.0f), QVector3D( 0.0f, 1.0f, 1.0f ));
}

SelectionBox::~SelectionBox()
{
    tearDown();
}

void SelectionBox::initialze()
{
    _program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/box_2d.vert");
    _program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/box_2d.frag");
    _program.link();
    _program.bind();

    // Cache Uniform Locations
    u_box_min = _program.uniformLocation("box_min");
    u_box_max = _program.uniformLocation("box_max");

    // Create Buffer (Do not release until VAO is created)
    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
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

void SelectionBox::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    Q_UNUSED(pMatrix)
    Q_UNUSED(vMatrix)
    Q_UNUSED(mMatrix)

    if (_select_mode == NONE)
        return;

    f->glDisable(GL_DEPTH_TEST);
    f->glDepthMask(GL_FALSE);

    _program.bind();
    _vbo.bind();

    _program.setUniformValue(u_box_min, _box_min);
    _program.setUniformValue(u_box_max, _box_max);
    {
        _vao.bind();
        f->glDrawArrays(GL_LINE_LOOP, 0, _model_vertexes.size());
        _vao.release();
    }
    _program.release();
}

void SelectionBox::tearDown()
{
    _vao.destroy();
    _vbo.destroy();
}
