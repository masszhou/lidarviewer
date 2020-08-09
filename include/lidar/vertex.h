#ifndef VERTEX_H
#define VERTEX_H
// from QtOpenGL tutorial by Trent Reed
// https://www.trentreed.net/topics/opengl/

#include <QVector3D>
#include <QMatrix4x4>
#include <QDebug>

class Vertex
{
public:
    // Constructor
    Vertex();
    explicit Vertex(const QVector3D &position);
    Vertex(const QVector3D &position, const QVector3D &color);
    Vertex(float x, float y, float z, float r, float g, float b);

    // Accessors
    const QVector3D& position() const;
    const QVector3D& color() const;
    void setPosition(const QVector3D& position);
    void setPosition(float x, float y, float z);
    void setColor(const QVector3D& color);
    void setColor(float r, float g, float b);

    // transform
    void rotate(const QMatrix4x4 m);
    void translate(const QVector3D v);

    // OpenGL Helper
    static const int PositionTupleSize = 3; // x, y, z
    static const int ColorTupleSize = 3;  // r, g, b
    static int positionOffset();
    static int colorOffset();
    static int stride();

    //

private:
    QVector3D m_position;
    QVector3D m_color;
};

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

// Constructor
inline Vertex::Vertex():m_position(0.0f, 0.0f, 0.0f), m_color(0.0f, 1.0f, 0.0f){}
inline Vertex::Vertex(const QVector3D &position): m_position(position), m_color(0.0f, 1.0f, 0.0f) {}
inline Vertex::Vertex(const QVector3D &position, const QVector3D &color): m_position(position), m_color(color){}
inline Vertex::Vertex(float x, float y, float z, float r, float g, float b): m_position(x,y,z), m_color(r,g,b){}

// Accessors
inline const QVector3D& Vertex::position() const {return m_position;}
inline const QVector3D& Vertex::color() const {return m_color;}
inline void Vertex::setPosition(const QVector3D& position) { m_position = position;}
inline void Vertex::setPosition(float x, float y, float z) { m_position = {x, y, z};}
inline void Vertex::setColor(const QVector3D& color) { m_color = color;}
inline void Vertex::setColor(float r, float g, float b){m_color = {r, g, b};}

inline void Vertex::rotate(const QMatrix4x4 m)
{
    QVector4D v = QVector4D(m_position, 1.0f);
    v = m * v;
    m_position = v.toVector3DAffine();
    // Returns the 3D vector form of this 4D vector,
    // dividing the x, y, and z coordinates by the w coordinate. Returns a null vector if w is zero.
}

inline void Vertex::translate(const QVector3D v)
{
    m_position = m_position + v;
}

// OpenGL Helper
inline int Vertex::positionOffset() { return offsetof(Vertex, m_position);}  // offsetof GCC macro
inline int Vertex::colorOffset() { return offsetof(Vertex, m_color);}
inline int Vertex::stride() { return sizeof(Vertex);}

#endif // VERTEX_H
