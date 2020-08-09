#ifndef VERTEX_STL_H
#define VERTEX_STL_H
#include <vector>
#include <stddef.h>

using namespace std;

class VertexSTL
{
public:
    // Constructor
    VertexSTL();
    explicit VertexSTL(const vector<float> &position);
    VertexSTL(const vector<float> &position, const vector<float> &color);
    VertexSTL(float x, float y, float z, float r, float g, float b);

    // Accessors
    const vector<float>& position() const;
    const vector<float>& color() const;
    void setPosition(const vector<float>& position);
    void setPosition(float x, float y, float z);
    void setColor(const vector<float>& color);

    // OpenGL Helper
    static const int PositionTupleSize = 3; // x, y, z
    static const int ColorTupleSize = 3;  // r, g, b
    static int positionOffset();
    static int colorOffset();
    static int stride();

private:
    vector<float> m_position;
    vector<float> m_color;
};

// Constructor
inline VertexSTL::VertexSTL():m_position(3, 0.0f), m_color(3, 0.0f){}
inline VertexSTL::VertexSTL(const vector<float> &position): m_position(position), m_color({0.0f, 1.0f, 0.0f}) {}
inline VertexSTL::VertexSTL(const vector<float> &position, const vector<float> &color): m_position(position), m_color(color){}
inline VertexSTL::VertexSTL(float x, float y, float z, float r, float g, float b): m_position({x,y,z}), m_color({r,g,b}){}

// Accessors
inline const vector<float>& VertexSTL::position() const {return m_position;}
inline const vector<float>& VertexSTL::color() const {return m_color;}
inline void VertexSTL::setPosition(const vector<float>& position) { m_position = position;}
inline void VertexSTL::setPosition(float x, float y, float z) { m_position = {x, y, z};}
inline void VertexSTL::setColor(const vector<float>& color) { m_color = color;}

// OpenGL Helper
inline int VertexSTL::positionOffset() { return offsetof(VertexSTL, m_position);}  // offsetof GCC macro
inline int VertexSTL::colorOffset() { return offsetof(VertexSTL, m_color);}
inline int VertexSTL::stride() { return sizeof(VertexSTL);}

#endif // VERTEX_STL_H
