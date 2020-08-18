// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_POINTCLOUD_H
#define LIDARVIEWER_POINTCLOUD_H
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <vector>

#include "vertex.h"
#include "octree.h"


class PointCloud
{
public:
    PointCloud();

    void loadPoints(std::vector<float>& positions);
    void loadColor(std::vector<float>& colors);

    void initialze();
    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);

    void tearDown();

private:
    void copyVertexesToVBO();
    bool getHeatMapColor(float value, float *red, float *green, float *blue);

private:
    // use std vector to keep compability with octree
    std::size_t _num_points;
    std::vector<float> _positions;
    std::vector<float> _colors;
    std::vector<float> _sizes;

    // shader
    QOpenGLBuffer _vbo;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram _program;
    int u_model_to_world;
    int u_world_to_camera;
    int u_camera_to_view;

    Octree _octree;
    QVector<Vertex> _vertexes;  // visible point cloud vertexes

    bool _is_ready;
    int _max_num_pts;  // -2,147,483,648 to 2,147,483,647
    float _point_size;
};

#endif // LIDARVIEWER_POINTCLOUD_H
