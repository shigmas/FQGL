#include "FQGLPlane.h"

FQGLPlane::FQGLPlane()
{
}

FQGLPlane::FQGLPlane(const QVector3D& v0, const QVector3D& v1,
                     const QVector3D& v2)
{
    QVector3D aux0 = v1 - v0;
    QVector3D aux1 = v2 - v0;
        
    _normal = QVector3D::crossProduct(aux0, aux1).normalized();
    _point = v2;
    _d = -QVector3D::dotProduct(_normal, _point);
}

float
FQGLPlane::DistanceFromPlane(const QVector3D& point) const
{
    return _d + QVector3D::dotProduct(_normal, point);

}
