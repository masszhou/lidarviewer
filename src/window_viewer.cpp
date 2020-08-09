#include "lidar/window_viewer.h"
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>

#include "lidar/vertex.h"

// Front Verticies
#define VERTEX_FTR Vertex( QVector3D( 0.5f,  0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f ) )
#define VERTEX_FTL Vertex( QVector3D(-0.5f,  0.5f,  0.5f), QVector3D( 0.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL Vertex( QVector3D(-0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 1.0f ) )
#define VERTEX_FBR Vertex( QVector3D( 0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 0.0f ) )

// Back Verticies
#define VERTEX_BTR Vertex( QVector3D( 0.5f,  0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL Vertex( QVector3D(-0.5f,  0.5f, -0.5f), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBL Vertex( QVector3D(-0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 0.0f, 1.0f ) )
#define VERTEX_BBR Vertex( QVector3D( 0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 1.0f ) )

// Create a colored cube
static const Vertex sg_vertexes[] = {
    // Face 1 (Front)
    VERTEX_FTR, VERTEX_FTL, VERTEX_FBL,
    VERTEX_FBL, VERTEX_FBR, VERTEX_FTR,
    // Face 2 (Back)
    VERTEX_BBR, VERTEX_BTL, VERTEX_BTR,
    VERTEX_BTL, VERTEX_BBR, VERTEX_BBL,
    // Face 3 (Top)
    VERTEX_FTR, VERTEX_BTR, VERTEX_BTL,
    VERTEX_BTL, VERTEX_FTL, VERTEX_FTR,
    // Face 4 (Bottom)
    VERTEX_FBR, VERTEX_FBL, VERTEX_BBL,
    VERTEX_BBL, VERTEX_BBR, VERTEX_FBR,
    // Face 5 (Left)
    VERTEX_FBL, VERTEX_FTL, VERTEX_BTL,
    VERTEX_FBL, VERTEX_BTL, VERTEX_BBL,
    // Face 6 (Right)
    VERTEX_FTR, VERTEX_FBR, VERTEX_BBR,
    VERTEX_BBR, VERTEX_BTR, VERTEX_FTR
};

#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR

ViewerWidget::ViewerWidget()
{
    m_transform.translate(0.0f, 0.0f, 0.0f);  // move back 5 units

    m_camera.initLookAtPoint(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
}

void ViewerWidget::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
    // void QOpenGLWidget::frameSwapped()
    // This signal is emitted after the widget's top-level window has finished composition
    // and returned from its potentially blocking QOpenGLContext::swapBuffers() call.
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    printVersionInformation();

    // Set global information
    glEnable(GL_CULL_FACE);  // only rendering front surfaces
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // set r,g,b = black, alpha = 1

    // Application-specific initialization
    {
        std::string vsCode =
                "#version 330\n"
                "layout(location = 0) in vec3 position;\n"
                "layout(location = 1) in vec3 color;\n"
                "out vec4 vColor;\n"
                "uniform mat4 modelToWorld;\n"
                "uniform mat4 worldToCamera;\n"
                "uniform mat4 cameraToView;\n"
                "void main(){\n"
                "    gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(position, 1.0);\n"
                "    vColor = vec4(color, 1.0);\n"
                "}\n";

        std::string fsCode =
                "#version 330\n"
                "in vec4 vColor;\n"
                "out vec4 fColor;\n"
                "void main(){\n"
                "    fColor = vColor;\n"
                "}\n";

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsCode.c_str());
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsCode.c_str());
        m_program->link();
        m_program->bind();

        // Cache Uniform Locations
        u_model_to_world = m_program->uniformLocation("modelToWorld");  // index
        u_world_to_camera = m_program->uniformLocation("worldToCamera");  // index
        u_camera_to_view = m_program->uniformLocation("cameraToView");  // index

        // Create Buffer (Do not release until VAO is created)
        m_vertex.create();
        m_vertex.bind();
        m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

        // Create Vertex Array Object
        m_object.create();
        m_object.bind();
        m_program->enableAttributeArray(0);
        m_program->enableAttributeArray(1);
        m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

        // Release (unbind) all
        m_object.release();
        m_vertex.release();
        m_program->release();
    }
}

void ViewerWidget::resizeGL(int width, int height)
{
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width / float(height), 0.1f, 1000.0f);
}

/**
 * @brief update the contents of the QOpenGLWidget's framebuffer
 */
void ViewerWidget::paintGL()
{
    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    m_program->bind();
    m_program->setUniformValue(u_world_to_camera, m_camera.getWorld2CameraMatrix());
    m_program->setUniformValue(u_camera_to_view, m_projection);
    {
        m_object.bind();
        m_program->setUniformValue(u_model_to_world, m_transform.getObject2WorldMatrix());
        glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes)/sizeof(sg_vertexes[0]));
        m_object.release();
    }
    m_program->release();
}


void ViewerWidget::teardownGL()
{
    // Actually destroy our OpenGL information
    m_object.destroy();
    m_vertex.destroy();
    delete  m_program;
}

/**
 * @brief slot function, call QOpenGLWidget::update() to invoke QOpenGLWidget::paintEvent then virtual paintGL()
 */
void ViewerWidget::update()
{

    // Update instance information
    // m_camera.rotate(5.0f, 0.0f);


    // Schedule a redraw
    QOpenGLWindow::update();
}

// =================================================
// interaction
void ViewerWidget::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_B){
        qDebug() << "getWorld2CameraMatrix";
        qDebug() << m_camera.getWorld2CameraMatrix();
        qDebug() << "getCameraPosition";
        qDebug() << m_camera.getCameraPosition();
        qDebug() << "getLookAtPosition";
        qDebug() << m_camera.getLookAtPosition();
        qDebug() << "getUpVector";
        qDebug() << m_camera.getUpVector();
    }
}

void ViewerWidget::wheelEvent(QWheelEvent *ev)
{
    // note: angleDelta() is in units of 1/8 degree
    m_camera.zoom(ev->angleDelta().y() / 120.0f);
    m_camera.save();
}

void ViewerWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        m_press_pos = ev->windowPos();
        m_camera.save();
    } else {
        QWindow::mousePressEvent(ev);
    }
}

void ViewerWidget::mouseMoveEvent(QMouseEvent *ev)
{
    // note: +x right, +y down
    if (ev->buttons() & Qt::LeftButton) {
        m_camera.restore();
        if (ev->modifiers() == Qt::ShiftModifier){
            m_camera.pan(QVector2D(ev->windowPos() - m_press_pos));
        }else if (ev->modifiers() == Qt::NoModifier){
            m_camera.rotate(QVector2D(ev->windowPos() - m_press_pos));
        }
    } else {
        QWindow::mouseMoveEvent(ev);
    }
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    QPointF releasePos = ev->windowPos();
    bool mouse_moved = releasePos != m_press_pos;
    if (mouse_moved) {
       m_camera.save();
    }
}


void ViewerWidget::printVersionInformation()
{
    QString glType;
    QString glVersion;

    // Get Version Information
    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    // qPrintable() will print our QString w/o quotes around it.
    qDebug() << qPrintable(glType) << qPrintable(glVersion);
}

