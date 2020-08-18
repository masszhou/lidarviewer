// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_SELECTIONBOX_H
#define LIDARVIEWER_SELECTIONBOX_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector>

#include "vertex.h"

class SelectionBox
{
public:
    enum SelectMode { ADD = 0, SUB = 1, NONE = 2 };

    SelectionBox();
    ~SelectionBox();

    void initialze();
    void render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix);

    void click(const QPointF p, const SelectMode select_mode);
    void drag(const QPointF p);
    void release();

    bool isActive() const;
    bool isEmpty() const;

    QRectF getBox() const;
    SelectMode getType() const;

    void tearDown();
private:
    // shader
    QOpenGLBuffer _vbo;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram _program;
    int u_box_min;
    int u_box_max;

    QVector<Vertex> _model_vertexes;

    SelectMode _select_mode;
    QPointF _anchor;  // GL canvas coordinate
    // here use QPointF instead of QrectF, since QRectF is image coordinate
    QPointF _box_min;  // GL canvas coordinate
    QPointF _box_max;  // GL canvas coordinate
};

inline void SelectionBox::click(const QPointF p, const SelectMode select_mode){
    _select_mode = select_mode;
    _anchor = p;
    _box_min = p;
    _box_max = p;
}
inline void SelectionBox::drag(const QPointF p){
    _box_min.setX(_anchor.x() < p.x() ? _anchor.x(): p.x());
    _box_min.setY(_anchor.y() < p.y() ? _anchor.y(): p.y());

    _box_max.setX(_anchor.x() > p.x() ? _anchor.x(): p.x());
    _box_max.setY(_anchor.y() > p.y() ? _anchor.y(): p.y());
}
inline void SelectionBox::release() {
    _select_mode = NONE;
    _box_min = _anchor;
    _box_max = _anchor;
}
inline bool SelectionBox::isActive() const { return _select_mode != NONE; }
inline bool SelectionBox::isEmpty() const { return _box_max == _box_min; }
inline QRectF SelectionBox::getBox() const {
    return QRectF(
                (_box_min.x() + 1.0)*0.5,
                (1.0-_box_max.y())*0.5,
                (_box_max.x()-_box_min.x())*0.5,
                (_box_max.y()-_box_min.y())*0.5);
}
inline SelectionBox::SelectMode SelectionBox::getType() const { return _select_mode; }

#endif // LIDARVIEWER_SELECTIONBOX_H
