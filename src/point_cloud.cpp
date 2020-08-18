#include "lidar/point_cloud.h"
#include <QtDebug>

#include <ctime>
#include <math.h>

PointCloud::PointCloud():
    _is_ready(false),
    _max_num_pts(1000000),
    _point_size(2.0f)
{

}

void PointCloud::loadPoints(std::vector<float> &positions)
{
    _positions.swap(positions);  // give data ownership to PointCloud instance
    _num_points = _positions.size() / 3;

    {
        clock_t begin = clock();
        _octree.buildTree(_positions, _sizes, 32);  // e.g. max_leaf_size = 32
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        qDebug() <<  "[PointCloud]: n_pts=" << _num_points << ", octree build time =" << elapsed_secs;
    }

    _vertexes.clear(); // release old vertexes

    std::size_t stride = 3;
    for(std::size_t i=0; i< _num_points; ++i){
        Vertex pt;
        pt.setPosition(_positions[i*stride], _positions[i*stride+1], _positions[i*stride+2]);
        pt.setColor(QVector3D(1.0f, 1.0f, 1.0f));
        _vertexes.append(pt);
    }
    qDebug() << "[PointCloud]: _vertexes.size() = " << _vertexes.size();

    if (_vertexes.size()==0){
        _is_ready = false;
        return;
    }else {
        copyVertexesToVBO();
        _is_ready = true;
    }
}

void PointCloud::clearPoints()
{
    _vertexes.clear();
    _is_ready = false;
}

void PointCloud::loadColor(std::vector<float> &colors)
{
    if (_is_ready){
        for (std::size_t i=0; i< colors.size(); ++i){
            _vertexes[i].setColor(0.0f, 0.5f+colors[i]/2, 0.2f+colors[i]/2);
        }
        copyVertexesToVBO();
    }
}

void PointCloud::initialze()
{
    //_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vsCode.c_str());  // add Vertex shader
    _program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/point.vert");  // add Vertex shader
    _program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/point.frag");  // add Fragment shader
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
    _vbo.allocate(static_cast<int>(sizeof(_vertexes[0])) * _max_num_pts);  // Allocate max size

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

    _is_ready = true;
}

void PointCloud::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    if (_is_ready){
        f->glEnable(GL_CULL_FACE);
        f->glEnable(GL_PROGRAM_POINT_SIZE);

        _program.bind();
        _program.setUniformValue(u_model_to_world, mMatrix);
        _program.setUniformValue(u_world_to_camera, vMatrix);
        _program.setUniformValue(u_camera_to_view, pMatrix);
        _program.setUniformValue("point_size", _point_size);
        {
            _vao.bind();
            f->glDrawArrays(GL_POINTS, 0, _vertexes.size());
            _vao.release();
        }
        _program.release();
    }
}

void PointCloud::tearDown()
{
    _vao.destroy();
    _vbo.destroy();
}

void PointCloud::copyVertexesToVBO()
{
    _is_ready = false;

    // update vbo for loaded points
    _vbo.bind();
    auto ptr = _vbo.map(QOpenGLBuffer::WriteOnly);
    memcpy(ptr, _vertexes.data(),  static_cast<unsigned int>(_vertexes.size()) * sizeof(_vertexes[0]));
    _vbo.unmap();
    _vbo.release();

    _is_ready = true;
}

//ref. http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
bool PointCloud::getHeatMapColor(float value, float *red, float *green, float *blue)
{
//    const int NUM_COLORS = 4;
//    static float color[NUM_COLORS][3] = { {0,0,1}, {0,1,0}, {1,1,0}, {1,0,0} };
//       A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.
    const int NUM_COLORS = 3;
    static float color[NUM_COLORS][3] = { {0.f, 0.f, 0.8f}, {0.8f, 0.8f, 0.8f}, {0.f,0.8f, 0.0f} };


    int idx1;        // |-- Our desired color will be between these two indexes in "color".
    int idx2;        // |
    float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

    if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
    else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
    else
    {
      value = value * (NUM_COLORS-1);        // Will multiply value by 3.
      idx1  = static_cast<int>(floorf(value));                  // Our desired color will be after this index.
      idx2  = idx1+1;                        // ... and before this index (inclusive).
      fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
    }

    *red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
    *green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
    *blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];
    return true;
}

