#include "dummy_floorgrid.h"


DummyFloorGrid::DummyFloorGrid():
    _visible(true),
    _grid_line_color(0.7f, 0.7f, 0.7f, 1.0f),
    _grid_floor_color(0.3f, 0.3f, 0.3f, 0.5f),
    _grid_floor_z(0.0f),
    _cell_size(1.0f),
    _line_weight(0.0f)
{
    loadSquare();
}

void DummyFloorGrid::initialze()
{    
    initializeShader(":/shader/floor_grid_ortho.vert", ":/shader/floor_grid_ortho.frag",
                     _program_ortho, _vbo_ortho, _vao_ortho,
                     _model_vertexes);

    initializeShader(":/shader/floor_grid_perspective.vert", ":/shader/floor_grid_perspective.frag",
                     _program_persp, _vbo_persp, _vao_persp,
                     _model_vertexes);
}

void DummyFloorGrid::initializeShader(const QString &vert_file, const QString &frag_file,
                                      QOpenGLShaderProgram &program, QOpenGLBuffer &vbo, QOpenGLVertexArrayObject &vao,
                                      const QVector<Vertex> &vertexes)
{
    program.addShaderFromSourceFile(QOpenGLShader::Vertex, vert_file);
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_file);
    program.link();
    program.bind();

    // Create Buffer (Do not release until VAO is created)
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertexes.data(), static_cast<int>(sizeof(Vertex)) * vertexes.size());

    // Create Vertex Array Object
    vao.create();
    vao.bind();
    program.enableAttributeArray(0);
    program.enableAttributeArray(1);
    program.setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    program.setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

    // Release (unbind) all
    vao.release();
    vbo.release();
    program.release();
}

void DummyFloorGrid::render(QOpenGLFunctions *f, const Camera camera)
{
//    renderOrtho(f, camera, 0.0f);
    renderPerspective(f, camera, 0.0f);
}

void DummyFloorGrid::renderOrtho(QOpenGLFunctions *f, const Camera camera, float z_floor)
{
    float t = camera.getCameraDistance() * tanf(0.5f * camera.getVerticalFOV());
    float r = camera.getAspectRatio() * t;

    float delta_pixels = 1.0f;  // in pixels;
    float delta_image = delta_pixels / camera.getImgHeight() / 1.0f * 2.0f * t;
    float eps_x, eps_y;

    float tan_theta = tanf(camera.getTheta());
    float sin_phi = sinf(camera.getPhi());
    float cos_phi = cosf(camera.getPhi());
    float tan_theta_2 = tan_theta * tan_theta;
    float sin_phi_2 = sin_phi * sin_phi;
    float cos_phi_2 = cos_phi * cos_phi;
    eps_x = delta_image * sqrtf(1.0f + cos_phi_2 / tan_theta_2);
    eps_y = delta_image * sqrtf(1.0f + sin_phi_2 / tan_theta_2);

    computeCellSize(_cell_size, _line_weight, camera, z_floor);

    QVector4D line_color;
    float a = 0.1f;
    float b = 0.2f;
    float fade_weight = (std::min)(1.2f, (std::max)(0.0f, (fabsf(sinf(camera.getTheta())) - a) / (b - a)));
    line_color = _grid_floor_color * (1.0f - fade_weight) + _grid_line_color * fade_weight;

    QVector3D eye, right, up, view;
    eye = camera.getCameraPosition();
    right = camera.getRightVector();
    up = camera.getUpVector();
    view = camera.getViewVector();

    _program_ortho.bind();
    _program_ortho.setUniformValue("eye", eye);
    _program_ortho.setUniformValue("right", right);
    _program_ortho.setUniformValue("up", up);
    _program_ortho.setUniformValue("view", view);
    _program_ortho.setUniformValue("t", t);
    _program_ortho.setUniformValue("r", r);
    _program_ortho.setUniformValue("z_floor", z_floor);
    _program_ortho.setUniformValue("eps_x", eps_x);
    _program_ortho.setUniformValue("eps_y", eps_y);
    _program_ortho.setUniformValue("cell_size", _cell_size);
    _program_ortho.setUniformValue("line_weight", _line_weight);
    _program_ortho.setUniformValue("line_color", line_color);
    _program_ortho.setUniformValue("floor_color", _grid_floor_color);
    {
        _vao_ortho.bind();
        f->glDrawArrays(GL_TRIANGLES, 0, _model_vertexes.size());
        _vao_ortho.release();
    }
    _program_ortho.release();
}

void DummyFloorGrid::renderPerspective(QOpenGLFunctions *f, const Camera camera, float z_floor)
{
    f->glDisable(GL_CULL_FACE);
    f->glDisable(GL_DEPTH_TEST);
    f->glDepthMask(GL_FALSE);
    f->glEnable(GL_BLEND);
    f->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

    computeCellSize(_cell_size, _line_weight, camera, z_floor);
    float weighted_cell_size = _cell_size * powf(10.0f, 1.0f - _line_weight);
    float max_dist_in_focus = visibleDistance(weighted_cell_size, 5.0f, camera, z_floor);
    float max_dist_visible = 2.0f * max_dist_in_focus;
    float h_lo, h_hi;
    computeHorizon(h_lo, h_hi, weighted_cell_size, camera, z_floor);

    float t = tanf(0.5f * camera.getVerticalFOV());
    float r = t * camera.getAspectRatio();

    float line_width =1.2f / camera.getImgHeight() / 1.0f * 2.0f * t;

    _program_persp.bind();
    _program_persp.setUniformValue("eye", camera.getCameraPosition());
    _program_persp.setUniformValue("right", camera.getRightVector());
    _program_persp.setUniformValue("left", camera.getLeftVector());
    _program_persp.setUniformValue("up", camera.getUpVector());
    _program_persp.setUniformValue("view", camera.getViewVector());
    _program_persp.setUniformValue("height", camera.getCameraPosition().y() - z_floor);
    _program_persp.setUniformValue("cell_size", _cell_size);
    _program_persp.setUniformValue("line_weight", _line_weight);
    _program_persp.setUniformValue("line_color", _grid_line_color);
    _program_persp.setUniformValue("floor_color", _grid_floor_color);
    _program_persp.setUniformValue("max_dist_in_focus", max_dist_in_focus);
    _program_persp.setUniformValue("max_dist_visible", max_dist_visible);
    _program_persp.setUniformValue("h_lo", h_lo);
    _program_persp.setUniformValue("h_hi", h_hi);
    _program_persp.setUniformValue("t", t);
    _program_persp.setUniformValue("r", r);
    _program_persp.setUniformValue("line_width", line_width);
    {
        _vao_persp.bind();
        f->glDrawArrays(GL_TRIANGLES, 0, _model_vertexes.size());
        _vao_persp.release();
    }
    _program_persp.release();
}

void DummyFloorGrid::tearDown()
{
    _vao_ortho.destroy();
    _vbo_ortho.destroy();
    _vao_persp.destroy();
    _vbo_persp.destroy();
}

void DummyFloorGrid::loadSquare()
{
//    _model_vertexes <<Vertex( QVector3D( 0.0f,   0.0f, 0.0f), QVector3D( 0.3f, 0.3f, 0.3f ));
//    _model_vertexes <<Vertex( QVector3D( 1.0f,   0.0f, 0.0f), QVector3D( 0.3f, 0.3f, 0.3f ));
//    _model_vertexes <<Vertex( QVector3D( 1.0f,   1.0f, 0.0f), QVector3D( 0.3f, 0.3f, 0.3f ));
//    _model_vertexes <<Vertex( QVector3D( 1.0f,   1.0f, 0.0f), QVector3D( 0.3f, 0.3f, 0.3f ));
//    _model_vertexes <<Vertex( QVector3D( 0.0f,   1.0f, 0.0f), QVector3D( 0.3f, 0.3f, 0.3f ));
//    _model_vertexes <<Vertex( QVector3D( 0.0f,   0.0f, 0.0f), QVector3D( 0.3f, 0.3f, 0.3f ));

    _model_vertexes <<Vertex( QVector3D( -0.5f,   -0.5f, 0.0f), QVector3D( 1.0f, 0.0f, 0.0f ));
    _model_vertexes <<Vertex( QVector3D( 0.5f,   -0.5f, 0.0f), QVector3D( 0.0f, 0.1f, 0.0f ));
    _model_vertexes <<Vertex( QVector3D( 0.5f,   0.5f, 0.0f), QVector3D( 0.0f, 0.0f, 1.0f ));
    _model_vertexes <<Vertex( QVector3D( 0.5f,   0.5f, 0.0f), QVector3D( 0.0f, 0.0f, 1.0f ));
    _model_vertexes <<Vertex( QVector3D( -0.5f,   0.5f, 0.0f), QVector3D( 0.5f, 0.5f, 0.5f ));
    _model_vertexes <<Vertex( QVector3D( -0.5f,   -0.5f, 0.0f), QVector3D( 1.0f, 0.0f, 0.0f ));
}

float DummyFloorGrid::normalizeAngle(float angle)
{
    // normalizes to interval [-pi,pi]
    angle = angle - floorf(angle / (2.0f * PI)) * 2.0f * PI;
    if (angle > PI) angle -= 2.0f * PI;
    return angle;
}

float DummyFloorGrid::visibleDistance(float cell_size, float projected_cell_size, const Camera &camera, float z_floor)
{
    // cell size in world space and projected cell size in pixels
    QVector3D eye = camera.getCameraPosition();
    float eye_floor_height = fabsf(eye.y() - z_floor);

    // DEBUG, here used fixed _win_height for debug
    return sqrtf(cell_size * eye_floor_height * camera.getImgHeight() / projected_cell_size / (2 * tanf(0.5f * camera.getVerticalFOV())));
}

bool DummyFloorGrid::computeHorizon(float &h_lo, float &h_hi, float cell_size, const Camera &camera, float z_floor)
{
    float projected_cell_size = 5.0f;
    float alpha = camera.getVerticalFOV();
    QVector3D eye = camera.getCameraPosition();
    float z_eye = eye.y();

    float eye_floor_height = z_eye - z_floor;
    float d_max = 2.0f * visibleDistance(cell_size, projected_cell_size, camera, z_floor);
    if (fabsf(eye_floor_height) > d_max) return false;
    float theta_max = asinf(eye_floor_height / d_max);  // TODO: handle eye_floor_height > d_max case
    float theta = normalizeAngle(camera.getTheta());

    Sector floor_sector;
    if (eye_floor_height < 0.0f)
        floor_sector = Sector(-PI - theta_max, theta_max);
    else
        floor_sector = Sector(theta_max, PI - theta_max);
    Sector camera_sector(theta - 0.5f * alpha, theta + 0.5f * alpha);
    Sector horizon_sector = camera_sector.intersect(floor_sector);
    if (horizon_sector.empty()) return false;
    h_lo = tanf(-horizon_sector.getEnd() + theta);
    h_hi = tanf(-horizon_sector.getStart() + theta);
    return true;
}

void DummyFloorGrid::computeCellSize(float &cell_size, float &line_weight, const Camera &camera, float z_floor)
{
    float projected_cell_size = 150.0f;
    float alpha = 45.0f / 180.0f * PI;

    QVector3D lookat = camera.getLookAtPosition();
    float d = camera.getCameraDistance() + fabsf(lookat.y() - z_floor);
    // DEBUG, here used fixed _win_height for debug
    float d_0 = camera.getImgHeight() / 2.0f / tanf(alpha / 2.0f) / projected_cell_size;
    float x = logf(d / d_0) / logf(10.0f);
    float x_ = floorf(x);
    line_weight = 1.0f - (x - x_);
    cell_size = powf(10.0f, x_);
}
