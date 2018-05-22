#include "CirclePrim.h"

#include <QQuaternion>
#include <QtMath>

CirclePrim::CirclePrim(float radius, int numSamples, const QVector4D& color) :
    _radius(radius),
    _numSamples(numSamples),
    _color(color)
{
}

CirclePrim::~CirclePrim()
{
}

void
CirclePrim::_CreateGeometry(FQGLPrimVertex **vertices,
                            uint &numVertices)
{
    // We're creating this as a triangle fan, which needs to close the last
    // point on the circle twice, and the center as well.

    // We'll take a normal vector, pointing out of the screen. We take the
    // radius, and get the points, as we rotate getting the sample points.
    QVector3D normal(0.0f, 0.0f, 1.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D firstPoint(0.0f, _radius, 0.0f);

    numVertices = _numSamples + 2;
    FQGLPrimVertex *verts = new FQGLPrimVertex[numVertices];
    int i = 0;
    verts[i].location = center;
    verts[i].color = _color;
    
    ++i;

    for ( ; i <= _numSamples ; ++i) {
        float rotation = (float)i/(float)_numSamples * 2 * M_PI;
        QQuaternion rotQuat =
            QQuaternion::fromAxisAndAngle(normal, qRadiansToDegrees(rotation));
        verts[i].location = rotQuat.rotatedVector(firstPoint);
        verts[i].color = _color;
    }
    verts[i].location = verts[1].location;
    verts[i].color = verts[1].color;

    *vertices = verts;
}
