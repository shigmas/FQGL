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
FQGL_DECLARE_PTRS(FQGLScene);
FQGL_DECLARE_PTRS(QOpenGLShaderProgram);
FQGL_DECLARE_PTRS(QOpenGLTexture);
FQGL_DECLARE_PTRS(QOpenGLVertexArrayObject);

class FQGLPrim : protected QOpenGLFunctions
{
public:
    FQGLPrim();
    virtual ~FQGLPrim();

    virtual void Initialize(FQGLScene * scene);

    void SetTextureById(GLuint textureId);
    void SetTextureByResourcePath(const QString& path);

    // draws this prim. if asTest is true, we render this for testing. This
    // may not have a difference for some prims.
    virtual void Render();

    virtual void SetTextureOffset(const QVector2D& textureOffset);

    virtual void SetTransform(const QMatrix4x4& transform);

    // Some convenience methods
    virtual void SetTranslate(const QVector3D& translate);

protected:
    enum _PrimMode {
        WireFramePrimMode,
        TexturePrimMode,
    };
    // Helper functions for subclasses
    QVector3D _GetScreenPointFromNDCPoint(const QVector3D& ndc) const;
    QVector3D _GetNDCPointFromScreenPoint(const QVector2D& screen) const;

    // Just drop the z, and convert to NDC space to texture space
    QVector2D _ScreenNdcToTex(const QVector3D& screen) const;
    
    QOpenGLTextureSharedPtr _InitializeTexture() const;

    // This function allocates the vertices, and sets numVertices. This is
    // called by Initialize, so the scene is set, if we need it to transform
    // points (to screen space, for instance). But, this method is only called
    // at initialization, and on not every camera movement.
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) = 0;

    // This function optionally allocates the points. If this isn't overridden,
    // we don't allocate the index buffer.
    virtual void _CreateIndices(GLuint **indices,
                                uint &numIndices);

    virtual GLenum _GetDrawMode(_PrimMode primMode) const = 0;

private:
    bool _isInitialized;
    FQGLPrimVertex * _vertices;
    uint  _numVertices;
    uint * _indices;
    uint  _numIndices;
    QString _texturePath;
    
    // If one of these two are set, and _ApplyTexture() returns true, we'll use
    // the texture. _textureId is used if we create the texture through some
    // other means, like the QOpenGLFramebufferObject. Otherwise, we'll pass in
    // a path that the texture object can use to create a QOpenGLTexture
    // instance.
    GLuint _textureId;
    QOpenGLTextureSharedPtr _texture;

    // Passed in in Initialize as this, so it's "safe".
    FQGLScene * _scene;

    QMatrix4x4 _transform;
    QVector2D _textureOffset;
    QOpenGLShaderProgramSharedPtr _primShader;
    QOpenGLBufferSharedPtr _vertexBuffer;
    QOpenGLBufferSharedPtr _indexBuffer;
    QOpenGLVertexArrayObjectSharedPtr _vao;
};

#endif
