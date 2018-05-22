#include "FQGLPrim.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <QDebug>

FQGLPrim::FQGLPrim() :
    _isInitialized(false),
    _vertices(NULL)
{
}



FQGLPrim::~FQGLPrim()
{
    if (_vertices) {
        delete _vertices;
    }
}

void
FQGLPrim::Initialize(QOpenGLShaderProgramSharedPtr sceneShader)
{
    initializeOpenGLFunctions();

    _CreateGeometry(&_vertices, _numVertices);

    _vao = QOpenGLVertexArrayObjectSharedPtr(new QOpenGLVertexArrayObject());
    _vao->create();
    _vao->bind();

    _vertexBuffer =
        QOpenGLBufferSharedPtr(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));
    _vertexBuffer->create();
    _vertexBuffer->bind();
    _vertexBuffer->allocate(_vertices, _numVertices * sizeof(FQGLPrimVertex));

    QOpenGLShaderProgramSharedPtr shader =
        _primShader ? _primShader : sceneShader;

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
FQGLPrim::Render(QOpenGLShaderProgramSharedPtr sceneShader, bool forTest)
{
    Q_UNUSED(forTest);

    QOpenGLShaderProgramSharedPtr shader =
        _primShader ? _primShader : sceneShader;

    _vao->bind();
    qDebug() << "Model transform: " << _transform;
    shader->setUniformValue("model", _transform);
    glDrawArrays(GL_TRIANGLE_FAN, 0, _numVertices);
}

void
FQGLPrim::SetTransform(const QMatrix4x4& transform)
{
    _transform = transform;
}
