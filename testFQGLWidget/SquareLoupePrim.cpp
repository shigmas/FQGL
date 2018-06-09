#include "SquareLoupePrim.h"

#include <QQuaternion>
#include <QtMath>

SquareLoupePrim::SquareLoupePrim(float length, float magnification,
                                 const QVector4D& color) :
    _length(length),
    _magnification(magnification),
    _color(color)
{
}

SquareLoupePrim::~SquareLoupePrim()
{
}

void
SquareLoupePrim::_CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices)
{
    QVector3D center(0.0f, 0.0f, 0.0f);
    float halfSize = _length/2.0f;

    numVertices = 4;
    int index = 0;
    FQGLPrimVertex *verts = new FQGLPrimVertex[numVertices];
    for (int i = -1 ; i <= 1 ; i += 2) {
        for (int j = -1 ; j <= 1 ; j += 2) {
            verts[index].location = QVector3D(center.x() + i*halfSize,
                                              center.y() + j*halfSize, 0.0f);
            // QVector3D screenNdc =
            //     _GetScreenPointFromNDCPoint(verts[index].location);
            verts[index].color = _color;
            //verts[index].texture = _ScreenNdcToTex(screenNdc);
            index++;
        }
    }

    verts[0].texture = QVector2D(0.0f, 0.0f);
    verts[1].texture = QVector2D(0.0f, 1.0f);
    verts[2].texture = QVector2D(1.0f, 0.0f);
    verts[3].texture = QVector2D(1.0f, 1.0f);

    *vertices = verts;
}

void
SquareLoupePrim::_CreateIndices(GLuint **indices, uint &numIndices)
{
    numIndices = 6;
    GLuint* inds = new uint[4];

    inds[0] = 0;
    inds[1] = 1;
    inds[2] = 2;
    inds[3] = 1;
    inds[4] = 2;
    inds[5] = 3;

    *indices = inds;
}

GLenum
SquareLoupePrim::_GetDrawMode(FQGLPrim::_PrimMode primMode) const
{
    Q_UNUSED(primMode);
    return GL_TRIANGLES;
    //return GL_LINE_STRIP;
}
