#include "lidar/dummy_car.h"
#include <QFile>

DummyCar::DummyCar():_point_size(1.0f)
{

}

DummyCar::~DummyCar(){
    tearDown();
}

void DummyCar::loadModel(const QString& passat_fileName, const QString& wheel_fileName, QVector<float> &vPoints, QVector<float> &tPoints, QVector<float> &nPoints)
{
    Q_UNUSED( vPoints )
    Q_UNUSED( tPoints )
    Q_UNUSED( nPoints )

    QStringList pieces;

    QFile passat_obj_file(passat_fileName);  // 210424 vertex
    passat_obj_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream passat_in(&passat_obj_file);
    QVector<QString> face;
    while (!passat_obj_file.atEnd()) {
        pieces = passat_in.readLine().split( " " );

        if (pieces[0] == "v"){  // 	Geometric vertices
            Vertex pt;
            pt.setPosition(pieces[1].toFloat(), pieces[2].toFloat(), pieces[3].toFloat());
            pt.setColor(QVector3D(0.0f, 1.0f, 0.0f));
            _vertexes_body.append(pt);
        }
    }
    passat_obj_file.close();

    QFile wheel_obj_file(wheel_fileName);
    wheel_obj_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream wheel_in(&wheel_obj_file);
    while (!wheel_obj_file.atEnd()) {
        pieces = wheel_in.readLine().split( " " );
        if (pieces[0] == "v"){  // 	Geometric vertices
            Vertex pt;
            pt.setPosition(pieces[1].toFloat(), pieces[2].toFloat(), pieces[3].toFloat());
            pt.setColor(QVector3D(0.8f, 1.0f, 0.0f));
            _vertexes_wheel.append(pt);
        }
    }
    wheel_obj_file.close();
}

void DummyCar::initialze()
{
    loadModel(":/3d_model/vw_passat_b8.pts",
              ":/3d_model/wheel.obj",
              _vertex_pts, _texture_pts, _normal_pts);

    initializeShader(":/shader/point.vert", ":/shader/point.frag", _program_body, _vbo_body, _vao_body, _vertexes_body);
    initializeShader(":/shader/point.vert", ":/shader/point.frag", _program_wheel, _vbo_wheel, _vao_wheel, _vertexes_wheel);
}

void DummyCar::initializeShader(const QString& vert_file, const QString& frag_file,
                                QOpenGLShaderProgram& program, QOpenGLBuffer& vbo, QOpenGLVertexArrayObject& vao,
                                const QVector<Vertex>& vertexes)
{
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, vert_file))
        qDebug() << "load/compile vertex shader failed";
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_file))
        qDebug() << "load/compile fragment shader failed";
    if (!program.link())
        qDebug() << "link shader pipeline failed";
    if (!program.bind())
        qDebug() << "bind shader pipeline for the current active shader failed";

    // Create Buffer (Do not release until VAO is created)
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    // load vertex data to VBO
    vbo.allocate(vertexes.data(), static_cast<int>(sizeof(Vertex)) * vertexes.size());

    // Create Vertex Array Object
    vao.create();
    vao.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    program.enableAttributeArray(0);
    program.enableAttributeArray(1);
    program.setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    program.setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

    // Release (unbind) all
    vao.release();
    vbo.release();
    program.release();

}

void DummyCar::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    f->glEnable(GL_CULL_FACE);
    // ---- draw body
    _program_body.bind();
    _program_body.setUniformValue("modelToWorld", mMatrix);
    _program_body.setUniformValue("worldToCamera", vMatrix);
    _program_body.setUniformValue("cameraToView", pMatrix);
    _program_body.setUniformValue("point_size", _point_size);
    {
        _vao_body.bind();
        f->glDrawArrays(GL_POINTS, 0, _vertexes_body.size());
        _vao_body.release();
    }
    _program_body.release();

    // ---- draw wheels
    _program_wheel.bind();
    _program_wheel.setUniformValue("worldToCamera", vMatrix);
    _program_wheel.setUniformValue("cameraToView", pMatrix);
    _program_wheel.setUniformValue("point_size", _point_size);
    {
        _vao_wheel.bind();
        // front right
        _program_wheel.setUniformValue("modelToWorld", mMatrix*QMatrix4x4(1.0f, 0.0f, 0.0f, 1.5f,
                                                                         0.0f, 1.0f, 0.0f, 0.3f,
                                                                         0.0f, 0.0f, 1.0f, 0.78f,
                                                                         0.0f, 0.0f, 0.0f, 1.0f));
        f->glDrawArrays(GL_POINTS, 0, _vertexes_wheel.size());
        // front left
        _program_wheel.setUniformValue("modelToWorld", mMatrix*QMatrix4x4(1.0f, 0.0f, 0.0f, 1.5f,
                                                                         0.0f, 1.0f, 0.0f, 0.3f,
                                                                         0.0f, 0.0f, -1.0f, -0.78f,
                                                                         0.0f, 0.0f, 0.0f, 1.0f));
        f->glDrawArrays(GL_POINTS, 0, _vertexes_wheel.size());
        // back right
        _program_wheel.setUniformValue("modelToWorld", mMatrix*QMatrix4x4(1.0f, 0.0f, 0.0f, -1.3f,
                                                                         0.0f, 1.0f, 0.0f, 0.3f,
                                                                         0.0f, 0.0f, 1.0f, 0.75f,
                                                                         0.0f, 0.0f, 0.0f, 1.0f));
        f->glDrawArrays(GL_POINTS, 0, _vertexes_wheel.size());
        // back left
        _program_wheel.setUniformValue("modelToWorld", mMatrix*QMatrix4x4(1.0f, 0.0f, 0.0f, -1.3f,
                                                                         0.0f, 1.0f, 0.0f, 0.3f,
                                                                         0.0f, 0.0f, -1.0f, -0.78f,
                                                                         0.0f, 0.0f, 0.0f, 1.0f));
        f->glDrawArrays(GL_POINTS, 0, _vertexes_wheel.size());
        _vao_wheel.release();
    }
    _program_wheel.release();
}

void DummyCar::tearDown()
{
    _vao_body.destroy();
    _vbo_body.destroy();
    _vao_wheel.destroy();
    _vbo_wheel.destroy();
}
