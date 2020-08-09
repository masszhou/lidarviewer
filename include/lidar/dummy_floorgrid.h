#ifndef DUMMYGRID_H
#define DUMMYGRID_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector>
#define PI 3.14159265359f

#include "camera.h"
#include "vertex.h"
#include "sector.h"


class DummyFloorGrid
{
public:
    DummyFloorGrid();
    ~DummyFloorGrid();

    void initialze();
    void initializeShader(const QString& vert_file, const QString& frag_file,
                          QOpenGLShaderProgram& program, QOpenGLBuffer& vbo, QOpenGLVertexArrayObject& vao,
                          const QVector<Vertex>& vertexes);

    void render(QOpenGLFunctions *f, const Camera camera);
    void renderOrtho(QOpenGLFunctions *f, const Camera camera, float z_floor);
    void renderPerspective(QOpenGLFunctions *f, const Camera camera, float z_floor);

    void tearDown();

protected:
    void loadSquare();

    float normalizeAngle(float angle);
    float visibleDistance(float cell_size, float projected_cell_size, const Camera& camera, float z_floor);
    bool computeHorizon(float& h_lo, float& h_hi, float cell_size, const Camera& camera, float z_floor);
    void computeCellSize(float& cell_size, float& line_weight, const Camera& camera, float z_floor);

private:
    // shader
    QOpenGLShaderProgram _program_ortho;
    QOpenGLBuffer _vbo_ortho;
    QOpenGLVertexArrayObject _vao_ortho;

    QOpenGLShaderProgram _program_persp;
    QOpenGLBuffer _vbo_persp;
    QOpenGLVertexArrayObject _vao_persp;

    QVector<Vertex> _model_vertexes;

    // grid properties
    bool _visible;
    QVector4D _grid_line_color;
    QVector4D _grid_floor_color;
    float _grid_floor_z;
    float _cell_size;
    float _line_weight;

};

inline DummyFloorGrid::~DummyFloorGrid(){ tearDown(); }

#endif // DUMMYGRID_H
