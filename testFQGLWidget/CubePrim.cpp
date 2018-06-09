#include "CubePrim.h"

#include <QQuaternion>
#include <QtMath>

CubePrim::CubePrim(float length, const QVector4D& color) :
    _length(length),
    _color(color)
{
}

CubePrim::~CubePrim()
{
}

void
CubePrim::_CreateGeometry(FQGLPrimVertex **vertices,
                            uint &numVertices)
{
    QVector3D center(0.0f, 0.0f, 0.0f);
    float halfSize = _length/2.0f;

    // This is a simple cube. To draw textures, etc., we'd have to have more
    // vertices.
    numVertices = 8;
    int index = 0;
    FQGLPrimVertex *verts = new FQGLPrimVertex[numVertices];
    for (int i = -1 ; i <= 1; i += 2) {
        // i = 0: bottom plane ; i = 1; top plane
        for (int j = -1 ; j <= 1 ; j += 2) {
            for (int k = -1 ; k <= 1 ; k += 2) {
                verts[index].location = QVector3D(center.x() + j*halfSize,
                                                  center.y() + i*halfSize,
                                                  center.z() + k*halfSize);
                verts[index].color = _color;
                ++index;
            }
        }
    }

    *vertices = verts;
}

void
CubePrim::_CreateIndices(GLuint **indices, uint &numIndices)
{
    numIndices = 14;
    GLuint* inds = new uint[14];

    inds[0] = 5;
    inds[1] = 7;
    inds[2] = 4;
    inds[3] = 6;
    inds[4] = 2;
    inds[5] = 7;
    inds[6] = 5;
    inds[7] = 1;
    inds[8] = 4;
    inds[9] = 0;
    inds[10] = 2;
    inds[11] = 1;
    inds[12] = 3;
    inds[13] = 5;

    *indices = inds;
}

GLenum
CubePrim::_GetDrawMode(FQGLPrim::_PrimMode primMode) const
{
    Q_UNUSED(primMode);

    return GL_TRIANGLE_STRIP;
}
