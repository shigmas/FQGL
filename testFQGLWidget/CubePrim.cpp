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

    inds[0] = 7;
    inds[1] = 6;
    inds[2] = 4;
    inds[3] = 5;
    inds[4] = 1;
    inds[5] = 6;
    inds[6] = 7;
    inds[7] = 3;
    inds[8] = 4;
    inds[9] = 0;
    inds[10] = 1;
    inds[11] = 3;
    inds[12] = 2;
    inds[13] = 7;

    *indices = inds;
}

GLenum
CubePrim::_GetDrawMode() const
{
    return GL_TRIANGLE_STRIP;
}
