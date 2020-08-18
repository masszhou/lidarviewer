#include "dummy_boxes.h"
#include <cmath>

// macro for cubic definition
// Front Verticies
#define VERTEX_FTR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X,  0.5f*H+Y,  0.5f*L+Z), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_FTL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X,  0.5f*H+Y,  0.5f*L+Z), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X, -0.5f*H+Y,  0.5f*L+Z), QVector3D( 0.0f, 0.1f, 1.0f ) )
#define VERTEX_FBR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X, -0.5f*H+Y,  0.5f*L+Z), QVector3D( 0.0f, 0.1f, 1.0f ) )

// Back Verticies
#define VERTEX_BTR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X,  0.5f*H+Y, -0.5f*L+Z), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X,  0.5f*H+Y, -0.5f*L+Z), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BBL(X,Y,Z,W,H,L) Vertex( QVector3D(-0.5f*W+X, -0.5f*H+Y, -0.5f*L+Z), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBR(X,Y,Z,W,H,L) Vertex( QVector3D( 0.5f*W+X, -0.5f*H+Y, -0.5f*L+Z), QVector3D( 0.0f, 1.0f, 1.0f ) )


DummyBoxes::DummyBoxes():
    _ready(false),
    _max_n_boxes(2400)
{
    // _max_n_boxes = 2400 -> 100 label boxes reserved
}

DummyBoxes::~DummyBoxes()
{
    tearDown();
}

void DummyBoxes::initialze()
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
    _vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    _vbo.allocate(_vertexes.data(), static_cast<int>(sizeof(_vertexes[0])) * _max_n_boxes);  // Allocate and upload

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

void DummyBoxes::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    if (_ready){
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
}

void DummyBoxes::tearDown()
{
    _vao.destroy();
    _vbo.destroy();
}

void DummyBoxes::loadBoxes(const QVector<float> &xyzwhlyaw_list)
{
    _vertexes.clear(); // release old vertexes

    uint32_t n_boxes = xyzwhlyaw_list.size()/7;
    for (uint32_t i=0; i < n_boxes; i++){
        float x = xyzwhlyaw_list[i*7];
        float y = xyzwhlyaw_list[i*7+1];
        float z = xyzwhlyaw_list[i*7+2];
        float w = xyzwhlyaw_list[i*7+3];
        float h = xyzwhlyaw_list[i*7+4];
        float l = xyzwhlyaw_list[i*7+5];
        float yaw = xyzwhlyaw_list[i*7+6];
        generateBox(x, y, z, w, h, l, yaw);
    }

    if (_vertexes.size()==0){
        _ready = false;
        return;
    }else {
        copyVertexesToVBO();
        _ready = true;
    }
}

void DummyBoxes::generateBox(float x, float y, float z, float w, float h, float l, float yaw)
{
    QMatrix4x4 rot_z(cos(-yaw),  -sin(-yaw), 0.0f, 0.0f,
                     sin(-yaw),  cos(-yaw),  0.0f, 0.0f,
                     0.0f,      0.0f,      1.0f, 0.0f,
                     0.0f,      0.0f,      0.0f, 1.0f);

    // defined for GL_LINES
    QVector<Vertex> box;

    // Face 1 (Front)
    box << VERTEX_FTR(x,y,z,w,h,l) << VERTEX_FTL(x,y,z,w,h,l);
    box << VERTEX_FTL(x,y,z,w,h,l) << VERTEX_FBL(x,y,z,w,h,l);
    box << VERTEX_FBL(x,y,z,w,h,l) << VERTEX_FBR(x,y,z,w,h,l);
    box << VERTEX_FBR(x,y,z,w,h,l) << VERTEX_FTR(x,y,z,w,h,l);

    // Face 2 (top)
    box << VERTEX_FTR(x,y,z,w,h,l) << VERTEX_BTR(x,y,z,w,h,l);
    box << VERTEX_BTL(x,y,z,w,h,l) << VERTEX_FTL(x,y,z,w,h,l);

    // Face 3 (Bottom)
    box << VERTEX_FBL(x,y,z,w,h,l) << VERTEX_BBL(x,y,z,w,h,l);
    box << VERTEX_BBR(x,y,z,w,h,l) << VERTEX_FBR(x,y,z,w,h,l);

    // Face 4 (Back)
    box << VERTEX_BBR(x,y,z,w,h,l) << VERTEX_BTR(x,y,z,w,h,l);
    box << VERTEX_BTR(x,y,z,w,h,l) << VERTEX_BTL(x,y,z,w,h,l);
    box << VERTEX_BTL(x,y,z,w,h,l) << VERTEX_BBL(x,y,z,w,h,l);
    box << VERTEX_BBL(x,y,z,w,h,l) << VERTEX_BBR(x,y,z,w,h,l);

    // rotate yaw in kitti velodyne coordinates
    QVector3D t(x, y, z);
    for (auto &v: box){
        v.translate(-t);
        v.rotate(rot_z);
        v.translate(t);
//        v.setColor(1.0f, 1.0f, 0.0f);
    }

    _vertexes.append(box);
}

void DummyBoxes::copyVertexesToVBO()
{
    _ready = false;

    // update vbo for loaded points
    _vbo.bind();
    auto ptr = _vbo.map(QOpenGLBuffer::WriteOnly);
    memcpy(ptr, _vertexes.data(),  static_cast<unsigned int>(_vertexes.size()) * sizeof(_vertexes[0]));
    _vbo.unmap();
    _vbo.release();

    _ready = true;
}


#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR
