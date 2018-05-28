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
    _textureId(0),
    //_texture(NULL),
    _primShader(NULL)
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

    _vao = QOpenGLVertexArrayObjectSharedPtr(new QOpenGLVertexArrayObject());
    _vao->create();
    _vao->bind();

    // Subclasses need to create the geometry. Get the vertices and populate
    // the vertex buffer
    _CreateGeometry(&_vertices, _numVertices);

    _vertexBuffer =
        QOpenGLBufferSharedPtr(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));
    _vertexBuffer->create();
    _vertexBuffer->bind();
    _vertexBuffer->allocate(_vertices, _numVertices * sizeof(FQGLPrimVertex));

    // Subclasses might also provide indices for drawing the geometry. The
    // default is not to have this array.
    _CreateIndices(&_indices, _numIndices);

    if (_numIndices > 0) {
        _indexBuffer =
            QOpenGLBufferSharedPtr(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer));
        _indexBuffer->create();
        _indexBuffer->bind();
        _indexBuffer->allocate(_indices, _numIndices * sizeof(GLuint));
    }

    
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
    // _texture = QOpenGLTextureSharedPtr(new QOpenGLTexture(QImage(path)));
    // //_texture->bind(bindingUnit);
    // _texture->setMinificationFilter(QOpenGLTexture::Linear);
    // _texture->setMagnificationFilter(QOpenGLTexture::Linear);
    // _texture->setWrapMode(QOpenGLTexture::ClampToEdge);
}

void
FQGLPrim::Render()
{
    _vao->bind();

    bool isTexSet = false;
    uint textureUnit = 1;
    if (_textureId) {
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glActiveTexture(textureUnit);
        isTexSet = true;
    // } else if (_texture) {
    //     _texture->bind(textureUnit);
    //     isTexSet = true;
    }

    // XXX - If we're using our own shader, the view or projection uniforms
    // haven't been set
    QOpenGLShaderProgramSharedPtr shader =
        _primShader ? _primShader : _scene->GetShader(!isTexSet);
    shader->bind();

    shader->setUniformValue("texture1", textureUnit);
    shader->setUniformValue("model", _transform);
    shader->setUniformValue("texOffset", _textureOffset);
    GLsizei numDrawVerts = _numIndices ? _numIndices : _numVertices;
    glDrawArrays(_GetDrawMode(), 0, numDrawVerts);
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

void
FQGLPrim::_CreateIndices(uint **indices, uint &numIndices)
{
    Q_UNUSED(indices);
    numIndices = 0;
}
