#include "FQGLPrim.h"

#include "FQGLScene.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <QDebug>

FQGLPrim::FQGLPrim() :
    _isInitialized(false),
    _vertices(NULL),
    _indices(NULL),
    _textureId(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
{
}



FQGLPrim::~FQGLPrim()
{
    if (_vertices) {
        delete [] _vertices;
     }
     if (_indices) {
         delete [] _indices;
     }
}

void
FQGLPrim::Initialize(FQGLScene * scene)
{
    _scene = scene;
    initializeOpenGLFunctions();

    if (_texturePath.size()) {
        _texture = _InitializeTexture();
    }

    _vao = std::make_shared<QOpenGLVertexArrayObject>();
    _vao->create();
    _vao->bind();

    // Subclasses need to create the geometry. Get the vertices and populate
    // the vertex buffer
    _CreateGeometry(&_vertices, _numVertices);

    _vertexBuffer = std::make_shared<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    _vertexBuffer->create();
    _vertexBuffer->bind();
    _vertexBuffer->allocate(_vertices, _numVertices * sizeof(FQGLPrimVertex));

    // Subclasses might also provide indices for drawing the geometry. The
    // default is not to have this array.
    _CreateIndices(&_indices, _numIndices);

    if (_numIndices > 0) {
        _indexBuffer =
            std::make_shared<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
        _indexBuffer->create();
        _indexBuffer->bind();
        _indexBuffer->allocate(_indices, _numIndices * sizeof(GLuint));
    }

    //    _vao->release();

    QOpenGLShaderProgramSharedPtr shader =
        _primShader ? _primShader : _scene->GetShader();

    shader->bind();
    int offset = 0;
    int stride = sizeof(FQGLPrimVertex);
    // For a numbered location in a shader, we're only using the program to
    // call glEnableVertexAttribArray()
    shader->enableAttributeArray(0);
    shader->setAttributeBuffer(0, GL_FLOAT, offset, 3, stride);

    offset += sizeof(QVector3D);
    shader->enableAttributeArray(1);
    shader->setAttributeBuffer(1, GL_FLOAT, offset, 4, stride);

    offset += sizeof(QVector4D);
    shader->enableAttributeArray(2);
    shader->setAttributeBuffer(2, GL_FLOAT, offset, 2, stride);

    _isInitialized = true;
}


void
FQGLPrim::SetTextureById(GLuint textureId)
{
    _textureId = textureId;
}

void
FQGLPrim::SetTextureByResourcePath(const QString& path)
{
    _texturePath = path;
}

void
FQGLPrim::Render()
{
    _vao->bind();

    bool hasTex = (_textureId != GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) ||
        _texture;

    // XXX - If we're the prim's shader, the view or projection uniforms
    // haven't been set. So, fix that if we use _primShader.
    QOpenGLShaderProgramSharedPtr shader =
        _primShader ? _primShader : _scene->GetShader(hasTex);
    shader->bind();

    if (_textureId != GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _textureId);
    } else if (_texture) {
        //_texture->bind(GL_TEXTURE0);
        _texture->bind(0);
    }

    //shader->setUniformValue("texture1", GL_TEXTURE0);
    shader->setUniformValue("model", _transform);
    shader->setUniformValue("texOffset", _textureOffset);
    if (_numIndices) {
        glDrawElements(_GetDrawMode(WireFramePrimMode), _numIndices,
                       GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(_GetDrawMode(WireFramePrimMode), 0, _numVertices);
    }
    _vao->release();
}

void
FQGLPrim::SetTextureOffset(const QVector2D& textureOffset)
{
    _textureOffset = textureOffset;
}

void
FQGLPrim::SetTransform(const QMatrix4x4& transform)
{
    _transform = transform;
}

void
FQGLPrim::SetTranslate(const QVector3D& translate)
{
    QMatrix4x4 mat;
    mat.translate(translate);
    SetTransform(mat);
}

QVector3D
FQGLPrim::_GetScreenPointFromNDCPoint(const QVector3D& ndc) const
{
    return _scene->GetScreenPointFromNDC(ndc);
}

QVector3D
FQGLPrim::_GetNDCPointFromScreenPoint(const QVector2D& screen) const
{
    return _scene->GetNDCPointFromScreen(screen);
}

QVector2D
FQGLPrim::_ScreenNdcToTex(const QVector3D& screen) const
{
    return QVector2D((screen.x() + 1.0f)/2.0f, (screen.y() + 1.0f)/2.0f);
}
 
QOpenGLTextureSharedPtr
FQGLPrim::_InitializeTexture() const
{
    QOpenGLTextureSharedPtr tex =
        std::make_shared<QOpenGLTexture>(QImage(_texturePath));
    // XXX - I think this might work with a texture registry in the scene.
    //_texture->bind(bindingUnit);
    tex->setMinificationFilter(QOpenGLTexture::Linear);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);

    return tex;
}

void
FQGLPrim::_CreateIndices(uint **indices, uint &numIndices)
{
    Q_UNUSED(indices);
    numIndices = 0;
}
