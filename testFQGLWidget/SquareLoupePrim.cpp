#include "SquareLoupePrim.h"

#include "FQGLScene.h"

#include <QQuaternion>
#include <QtMath>

SquareLoupePrim::SquareLoupePrim(float length, float magnification,
                                 const QVector4D& color) :
    FQGLPrim(FQGLScreenViewType),
    _length(length),
    _magnification(magnification),
    _color(color)
{
    SetAsStencil(true);
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

    Q_UNUSED(_magnification);

    numVertices = 4;
    int index = 0;
    FQGLPrimVertex *verts = new FQGLPrimVertex[numVertices];
    FQGLScene * scene = _GetScene();
    // We just use 0 for the z. Screen space prims will be drawn in
    // orthographic, so we can easily move the z when we draw.
    for (int i = -1 ; i <= 1 ; i += 2) {
        for (int j = -1 ; j <= 1 ; j += 2) {
            QVector2D scene2d(center.x() + i*halfSize,
                              center.y() + j*halfSize);
            //verts[index].location = QVector3D(coord, 2.89f);
            // This depth depends on the other prim - we want it to be at the
            // same distance, since we want to be the equivalent size. We're
            // really in the screen (since we're a stencil), but we want the
            // size to be the same.
            verts[index].location = QVector3D(scene2d, 0.0);
            verts[index].color = _color;
            QVector2D coord =
                scene->GetCoordinateFromScene(verts[index].location);
            verts[index].texture =
                _TexToOffset(_CoordToTex(coord))/_magnification;
            index++;
        }
    }

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
