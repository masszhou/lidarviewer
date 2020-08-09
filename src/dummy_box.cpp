#include "lidar/dummy_box.h"

// macro for cubic definition
// Front Verticies
#define VERTEX_FTR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X,  0.5f*H+Y,  0.5f*L+Z), QVector3D( 1.0f, 0.0f, 0.0f ) )
#define VERTEX_FTL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X,  0.5f*H+Y,  0.5f*L+Z), QVector3D( 0.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X, -0.5f*H+Y,  0.5f*L+Z), QVector3D( 0.0f, 0.0f, 1.0f ) )
#define VERTEX_FBR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X, -0.5f*H+Y,  0.5f*L+Z), QVector3D( 0.3f, 0.3f, 0.3f ) )

// Back Verticies
#define VERTEX_BTR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X,  0.5f*H+Y, -0.5f*L+Z), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X,  0.5f*H+Y, -0.5f*L+Z), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X, -0.5f*H+Y, -0.5f*L+Z), QVector3D( 1.0f, 0.0f, 1.0f ) )
#define VERTEX_BBR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X, -0.5f*H+Y, -0.5f*L+Z), QVector3D( 1.0f, 1.0f, 1.0f ) )

DummyBox::DummyBox():
    _position(0.0f, 0.0f, 0.0f), _dims(1.0f, 1.0f, 1.0f)
{
    generateBox(_position, _dims);
}

DummyBox::DummyBox(float x, float y, float z, float w, float h, float l):
    _position(x, y, z), _dims(w, h, l)
{
    generateBox(_position, _dims);
}

void DummyBox::generateBox(QVector3D position, QVector3D dims)
{
    //      y           w
    //      |          /
    //     / -- x   -----l  default camera is x axis to negative direction
    //    z           |
    //                h
    float x = position.x();
    float y = position.y();
    float z = position.z();
    float w = dims.x();
    float h = dims.y();
    float l = dims.z();

    // Face 1 (Front)
    _model_vertexes << VERTEX_FTR(x,y,z,w,h,l) << VERTEX_FTL(x,y,z,w,h,l) << VERTEX_FBL(x,y,z,w,h,l) << VERTEX_FBR(x,y,z,w,h,l) << VERTEX_FTR(x,y,z,w,h,l);

    // Face 2 (top)
    _model_vertexes << VERTEX_FTR(x,y,z,w,h,l) << VERTEX_BTR(x,y,z,w,h,l) << VERTEX_BTL(x,y,z,w,h,l) << VERTEX_FTL(x,y,z,w,h,l);

    // Face 3 (Bottom)
    _model_vertexes << VERTEX_FBL(x,y,z,w,h,l) << VERTEX_BBL(x,y,z,w,h,l) << VERTEX_BBR(x,y,z,w,h,l) << VERTEX_FBR(x,y,z,w,h,l);

    // Face 4 (Back)
    _model_vertexes << VERTEX_FBR(x,y,z,w,h,l) << VERTEX_BBR(x,y,z,w,h,l) << VERTEX_BTR(x,y,z,w,h,l) << VERTEX_BTL(x,y,z,w,h,l) << VERTEX_BBL(x,y,z,w,h,l);

}

DummyBox::~DummyBox(){
    tearDown();
}

void DummyBox::initialze()
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
    _vbo.allocate(_model_vertexes.data(), static_cast<int>(sizeof(_model_vertexes[0])) * _model_vertexes.size());  // Allocate and upload

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

void DummyBox::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    f->glEnable(GL_CULL_FACE);

    _program.bind();
    _program.setUniformValue(u_model_to_world, mMatrix);
    _program.setUniformValue(u_world_to_camera, vMatrix);
    _program.setUniformValue(u_camera_to_view, pMatrix);
    {
        _vao.bind();
        f->glDrawArrays(GL_LINE_STRIP, 0, _model_vertexes.size());
        _vao.release();
    }
    _program.release();
}

void DummyBox::tearDown()
{
    _vao.destroy();
    _vbo.destroy();
}


#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR

