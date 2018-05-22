#ifndef FQGL_PRIM
#define FQGL_PRIM

#include "FQGLTypes.h"

#include <QOpenGLFunctions>

#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

struct FQGLPrimVertex
{
    QVector3D location;
    QVector4D color;
    QVector2D texture;
};

FQGL_DECLARE_PTRS(QOpenGLBuffer);
FQGL_DECLARE_PTRS(QOpenGLShaderProgram);
FQGL_DECLARE_PTRS(QOpenGLVertexArrayObject);

class FQGLPrim : protected QOpenGLFunctions
{
public:
    FQGLPrim();
    virtual ~FQGLPrim();

    virtual void Initialize(QOpenGLShaderProgramSharedPtr sceneShader = NULL);

    // draws this prim. if asTest is true, we render this for testing. This
    // may not have a difference for some prims.
    virtual void Render(QOpenGLShaderProgramSharedPtr sceneShader = NULL,
                        bool forTest=false);

    virtual void SetTransform(const QMatrix4x4& transform);

protected:
    // This function allocates the vertices, and sets numVertices
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) = 0;    

private:
    bool _isInitialized;
    FQGLPrimVertex * _vertices;
    uint  _numVertices;

    QMatrix4x4 _transform;
    QOpenGLShaderProgramSharedPtr _primShader;
    QOpenGLBufferSharedPtr _vertexBuffer;
    QOpenGLVertexArrayObjectSharedPtr _vao;
};

#endif
