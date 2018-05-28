#include "FQGLScene.h"

#include "FQGLCamera.h"
#include "FQGLFrustum.h"
#include "FQGLPrim.h"

#include <QOpenGLShaderProgram>

#include <QtMath>

#include <algorithm>

FQGLScene::FQGLScene(int width, int height) :
    _basicShader(NULL,
                 [] (QOpenGLShaderProgram *prog) {
                     qDebug() << "Custom deleter for QOpenGLShaderProgram";
                     if (prog) {
                         prog->release();
                         delete prog;
                     } else {
                         qDebug() << "_basicShader is NULL";
                     }
                 }),
    _textureShader(NULL,
                 [] (QOpenGLShaderProgram *prog) {
                       qDebug() << "Custom deleter for QOpenGLShaderProgram";
                       if (prog) {
                         prog->release();
                         delete prog;
                       } else {
                           qDebug() << "_textureShader is NULL";
                       }
                   }),
    //_camera(new FQGLCamera(QVector3D(0.0f, 0.0f, 3.0f)))
    _camera(new FQGLCamera(QVector3D(0.0f, 0.0f, 3.0f)),
            [] (FQGLCamera *) {
                qDebug() << "Custom deleter for FQGLCamera";
            })
{
    if (width && height) {
        _camera->SetPerspective(width, height, 45.0f, 0.1f, 100.0f);
    }        
    for (int i = 0 ; i < 6 ; ++i) {
        _shaders[i] = NULL;
    }
}

FQGLScene::FQGLScene(const char * vertexShader,
                     const char * basicShader,
                     const char * textureShader) :
    _basicShader(NULL),
    _textureShader(NULL),
    _camera(new FQGLCamera(QVector3D(0.0f, 0.0f, 3.0f)))
    // _camera(new FQGLCamera(QVector3D(0.0f, 0.0f, 3.0f)),
    //         std::default_delete<FQGLCamera>())
{
    _shaders[0] = vertexShader;
    _shaders[1] = basicShader;
    _shaders[2] = textureShader;
}

FQGLScene::~FQGLScene()
{
    qDebug() << "Byebye";
}

void
FQGLScene::SetPerspective(int width, int height)
{
    float aspect = fabs(width/height);
    //    _camera->SetPerspective(width, height, 45.0f, 0.1f, 100.0f);
    _camera->SetPerspective(aspect, 45.0f, 0.1f, 100.0f);
}

void
FQGLScene::SetShaders(const char * vertexShader,
                      const char * basicShader,
                      const char * textureShader)
{
    _shaders[0] = vertexShader;
    _shaders[1] = basicShader;
    _shaders[2] = textureShader;
}

void
FQGLScene::Initialize()
{
    initializeOpenGLFunctions();

    QOpenGLShaderProgram * basic = InitShaders(_shaders[0], _shaders[1]);
    if (basic) {
        _basicShader = QOpenGLShaderProgramSharedPtr(basic);
    } else {
        qDebug() << "Unable to link basic shader";
    }

    QOpenGLShaderProgram * tex = InitShaders(_shaders[0], _shaders[2]);
    if (tex) {
        _textureShader = QOpenGLShaderProgramSharedPtr(tex);
    } else {
        qDebug() << "Unable to link texture shader";
    }

    // I don't think these need to be bound to anything
    glGenQueries(2, _queryIds);

    _InitPrims();
}

void
FQGLScene::AddPrim(const FQGLPrimSharedPtr& prim, bool asStencilPrim)
{
    if (asStencilPrim) {
        _stencilPrims.push_back(prim);
    } else {
        _prims.push_back(prim);
    }
}

QOpenGLShaderProgramSharedPtr
FQGLScene::GetShader(bool basicShader) const
{
    return basicShader ? _basicShader : _textureShader;
}

void
FQGLScene::Render(const QVector4D& clearColor,
                  FQGLFramebufferType frameBufferType)
{
    GLbitfield clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    if (frameBufferType != FQGLDefaultFramebufferType) {
        // Let's clear the stencil too
        clearBits |= GL_STENCIL_BUFFER_BIT;
    }
    glClearColor(clearColor.w(), clearColor.x(), clearColor.x(),
                 clearColor.z());
    glClear(clearBits);

    // If the prim has a shader, we should get it and set our variables here.
    _basicShader->bind();
    _basicShader->setUniformValue("view", _camera->GetViewMatrix());
    _basicShader->setUniformValue("projection",
                                  _camera->GetProjectionMatrix());
    _textureShader->bind();
    _textureShader->setUniformValue("view", _camera->GetViewMatrix());
    _textureShader->setUniformValue("projection",
                                    _camera->GetProjectionMatrix());

    // Always draw stencil prims first. Definitely makes a difference in the
    // testing case.
    if (frameBufferType != FQGLDefaultFramebufferType) {
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }
    std::for_each(_stencilPrims.cbegin(), _stencilPrims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      _RenderPrim(prim, false);
                  });


    if (frameBufferType != FQGLDefaultFramebufferType) {
        glStencilMask(0x00);
        // Either GL_NOTEQUAL or GL_GREATER work for this
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    }
    std::for_each(_prims.cbegin(), _prims.cend(),
                  [this, frameBufferType] (const FQGLPrimSharedPtr &prim) {
                      bool doTest = frameBufferType == FQGLPickingFramebufferType;
                      bool didPass = _RenderPrim(prim, doTest);
                      if (doTest) {
                          qDebug() << "Passed: " << didPass;
                      }
                  });


}

QVector3D
FQGLScene::GetCameraScreenPoint(const QVector2D& screenPoint) const
{
    FQGLFrustum frustum = _camera->GetFrustum();

    return frustum.GetPointInFrustum(_camera->GetUp(), screenPoint,
                                     _camera->GetAspectRatio());
}

QOpenGLShaderProgram *
FQGLScene::InitShaders(const char * vshader,
                       const char * fshader)
{
    QOpenGLShaderProgram *program = new QOpenGLShaderProgram;

    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          vshader)) {
        return NULL;
    }

    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          fshader)) {
        qDebug() << "fshader failed";
        return NULL;
    }

    // if (geomShader) {
    //     if (!program->addShaderFromSourceFile(QOpenGLShader::Geometry,
    //                                           geomShader)) {
    //         qDebug() << "fshader failed";
    //         return NULL;
    //     }
    // }

    // if (tessControlShader) {
    //     if (!program->addShaderFromSourceFile(QOpenGLShader::TessellationControl,
    //                                           tessControlShader)) {
    //         qDebug() << "tessControl shader failed";
    //         return NULL;
    //     }
    // }

    // if (tessEvalShader) {
    //     if (!program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation,
    //                                           tessEvalShader)) {
    //         qDebug() << "tessEval fshader failed";
    //         return NULL;
    //     }
    // }

    // if (computeShader) {
    //     if (!program->addShaderFromSourceFile(QOpenGLShader::Compute,
    //                                           computeShader)) {
    //         qDebug() << "compute  failed";
    //         return NULL;
    //     }
    // }

    if (!program->link()) {
        qDebug() << "shader link failed";
        return NULL;
    }

    return program;
}

void
FQGLScene::_InitPrims()
{
    std::for_each(_prims.cbegin(), _prims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      prim->Initialize(this);
                  });
    std::for_each(_stencilPrims.cbegin(), _stencilPrims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      prim->Initialize(this);
                  });
}

bool
FQGLScene::_RenderPrim(const FQGLPrimSharedPtr& prim, bool isTesting,
                       TestType testType)
{
    if (isTesting) {
        glBeginQuery(GL_SAMPLES_PASSED, _queryIds[0]);
        glBeginQuery(GL_ANY_SAMPLES_PASSED, _queryIds[0]);
    }

    prim->Render();

    if (isTesting) {
        glEndQuery(GL_SAMPLES_PASSED);
        glEndQuery(GL_ANY_SAMPLES_PASSED);

        // What were the results of the query?
        GLuint anySamplesPassed = 0;
        GLuint samplesPassed = 0;
        glGetQueryObjectuiv(_queryIds[0], GL_QUERY_RESULT, &anySamplesPassed);
        glGetQueryObjectuiv(_queryIds[1], GL_QUERY_RESULT, &samplesPassed);

        // Now, depending on the testType, return true or false;
        switch (testType) {
        case AnySamplePassedTestType:
            return !(anySamplesPassed != 0);
            break;
        case AllSamplePassedTestType:
            return !(anySamplesPassed == 0);
            break;
        }
    } else {
        return false;
    }
}
