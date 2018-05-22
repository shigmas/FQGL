#ifndef FQGL_PLANE
#define FQGL_PLANE

#include <QVector3D>

class FQGLPlane
{
public:
    FQGLPlane();
    // Initializes the plane with an array of 3 points.  We want the normal to
    // be facing in the right way (i.e. the plane )
    FQGLPlane(const QVector3D& v0, const QVector3D& v1,
              const QVector3D& v2);

    // with a given definition of the plane (by the 3 points), which define
    // a normal, this can be negative.  Useful for determining which side of
    // the plane the point is on.
    float DistanceFromPlane(const QVector3D& point) const;

private:
    QVector3D _normal;
    QVector3D _point;
    float _d;
};

#endif // FQGL_PLANE
