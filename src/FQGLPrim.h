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
// class QOpenGLShaderProgram;
// #define QOpenGLShaderProgramSharedPtr QOpenGLShaderProgram*
FQGL_DECLARE_PTRS(QOpenGLTexture);
FQGL_DECLARE_PTRS(QOpenGLVertexArrayObject);

class FQGLPrim : protected QOpenGLFunctions
{
public:
    FQGLPrim(FQGLPrimViewType viewType=FQGLSceneViewType);
    virtual ~FQGLPrim();

    // Releases any resources that are owned by this object, excluding memory,
    // which will be done in the destructor.
    virtual void Release();

    FQGLPrimViewType GetViewType() const;

    bool IsStencil() const;
    void SetAsStencil(bool isStencil);

    virtual void Initialize(FQGLScene * scene);

    void SetTextureById(GLuint textureId);
    void SetTextureByResourcePath(const QString& path);

    // draws this prim. if asTest is true, we render this for testing. This
    // may not have a difference for some prims.
    virtual void Render();

    // Sets the center of the texture. (The offsets of this are in a VBO
    virtual void SetTextureCenter(const QVector2D& textureCenter);

    virtual void SetTransform(const QMatrix4x4& transform);

    // Some convenience methods
    virtual void SetTranslate(const QVector3D& translate);

protected:
    enum _PrimMode {
        WireFramePrimMode,
        TexturePrimMode,
    };

    // -------
    // Helper functions for subclasses

    FQGLScene * _GetScene() const;
    
    //QOpenGLTextureSharedPtr _InitializeTexture() const;
    QOpenGLTexture * _InitializeTexture() const;

    // Textures are from TopLeft (0,0) to Bottom Right (1,1), So, take a
    // coordinate point and map it to a texture point.
    QVector2D _CoordToTex(const QVector2D& coord) const;

    QVector2D _TexToOffset(const QVector2D& coord) const;

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
    FQGLPrimViewType _viewType;
    bool _isStencil;
    QVector3D _center;
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
    //QOpenGLTextureSharedPtr _texture;
    QOpenGLTexture * _texture;

    // Passed in in Initialize as this, so it's "safe".
    FQGLScene * _scene;

    QMatrix4x4 _transform;
    QVector2D _textureCenter;
    QOpenGLShaderProgramUniquePtr _primShader;
    QOpenGLBufferUniquePtr _vertexBuffer;
    QOpenGLBufferUniquePtr _indexBuffer;
    QOpenGLVertexArrayObjectUniquePtr _vao;
};

#endif
