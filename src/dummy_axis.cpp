#include "lidar/dummy_axis.h"

dummy_axis::dummy_axis()
{

}

dummy_axis::dummy_axis(float cx, float cy, float cz, QVector3D x_basis, QVector3D y_basis, QVector3D z_basis)
{
    Q_UNUSED( cx )
    Q_UNUSED( cy )
    Q_UNUSED( cz )
    Q_UNUSED( x_basis )
    Q_UNUSED( y_basis )
    Q_UNUSED( z_basis )
}

void dummy_axis::initialze()
{

}

void dummy_axis::render(QOpenGLFunctions *f, const QMatrix4x4 &pMatrix, const QMatrix4x4 &vMatrix, const QMatrix4x4 &mMatrix)
{
    Q_UNUSED( f )
    Q_UNUSED( pMatrix )
    Q_UNUSED( vMatrix )
    Q_UNUSED( mMatrix )
}

void dummy_axis::tearDown()
{

}
