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
    QVector3D firstLocPoint(0.0f, _radius, 0.0f);
    // We need the length in screen points (not at the origin) of the radius
    // with magnification. Then make that the first texture point.
    // XXX - this all just works because the camera is currently pointing at the
    // origin, and the circle is at the origin, so all this works fine in the
    // screen frustum.
    numVertices = _numSamples + 2;
    FQGLPrimVertex *verts = new FQGLPrimVertex[numVertices];
    int i = 0;
    verts[i].location = locCenter;
    verts[i].color = _color;
    verts[i].texture = _ScreenNdcToTex(locCenter);
    
    ++i;
    for ( ; i <= _numSamples ; ++i) {
        float rotation = (float)i/(float)_numSamples * 2 * M_PI;

        QQuaternion posRot =
            QQuaternion::fromAxisAndAngle(normal, qRadiansToDegrees(rotation));
        verts[i].location = posRot.rotatedVector(firstLocPoint) + locCenter;
        verts[i].color = _color;
        // We'll translate these to camera points on render
        // Convert location to a screen point
        QVector3D screenNdc = _GetScreenPointFromNDCPoint(verts[i].location);
        screenNdc /= _magnification;
        verts[i].texture = _ScreenNdcToTex(screenNdc);
        //qDebug() << i << "tex: " << verts[i].texture;
    }
    verts[i].location = verts[1].location;
    verts[i].color = verts[1].color;
    verts[i].texture = verts[1].texture;

    *vertices = verts;
}

GLenum
LoupePrim::_GetDrawMode(FQGLPrim::_PrimMode primMode) const
{
    Q_UNUSED(primMode);
    return GL_TRIANGLE_FAN;
}
