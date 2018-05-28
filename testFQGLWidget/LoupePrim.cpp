#include "LoupePrim.h"

#include <QQuaternion>
#include <QtMath>

LoupePrim::LoupePrim(float radius, int numSamples, float magnification,
                     const QVector4D& color) :
    _radius(radius),
    _numSamples(numSamples),
    _magnification(magnification),
    _color(color)
{
}

LoupePrim::~LoupePrim()
{
}

void
LoupePrim::_CreateGeometry(FQGLPrimVertex **vertices,
                            uint &numVertices)
{
    // We're creating this as a triangle fan, which needs to close the last
    // point on the circle twice, and the center as well.

    // We'll take a normal vector, pointing out of the screen. We take the
    // radius, and get the points, as we rotate getting the sample points.
    QVector3D normal(0.0f, 0.0f, 1.0f);
    QVector3D locCenter(0.0f, 0.0f, 0.0f);
    // Quaternions work in 3D space, so we have the Z, which we'll drop
    // when we set it.
    QVector3D texCenter(0.5f, 0.5f, 0.0);
    QVector3D firstLocPoint(0.0f, _radius, 0.0f);
    QVector3D firstTexPoint(0.0f, _radius * _magnification, 0.0f);

    numVertices = _numSamples + 2;
    FQGLPrimVertex *verts = new FQGLPrimVertex[numVertices];
    int i = 0;
    verts[i].location = locCenter;
    verts[i].color = _color;
    verts[i].texture = QVector2D(texCenter);
    
    ++i;
    for ( ; i <= _numSamples ; ++i) {
        float rotation = (float)i/(float)_numSamples * 2 * M_PI;
        QQuaternion rotQuat =
            QQuaternion::fromAxisAndAngle(normal, qRadiansToDegrees(rotation));
        verts[i].location = rotQuat.rotatedVector(firstLocPoint) + locCenter;
        verts[i].color = _color;
        verts[i].texture = QVector2D(rotQuat.rotatedVector(firstTexPoint) +
                                     texCenter);
    }
    verts[i].location = verts[1].location;
    verts[i].color = verts[1].color;
    verts[i].texture = verts[1].texture;

    *vertices = verts;
}

GLenum
LoupePrim::_GetDrawMode() const
{
    return GL_TRIANGLE_FAN;
}
