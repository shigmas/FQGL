#include "FQGLScene.h"

#include "FQGLFrustum.h"
#include "FQGLPrim.h"

#include <QOpenGLShaderProgram>

#include <QtMath>

#include <algorithm>

FQGLScene::_PrimRenderState::_PrimRenderState(bool isStencil,
                                              QOpenGLFunctions* functions) :
    _isStencil(isStencil),
    _functions(functions)
{
    if (_isStencil) {
        _functions->glStencilMask(0xFF);
        _functions->glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }
}

FQGLScene::_PrimRenderState::~_PrimRenderState()
{
    if (_isStencil) {
        _functions->glStencilMask(0x00);
        // Either GL_NOTEQUAL or GL_GREATER work for this
        _functions->glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    }
}

FQGLScene::FQGLScene(int width, int height) :
    _camera(QVector3D(0.0f, 0.0f, 3.0f)),
    _screenSpaceCamera(QVector3D(0.0f, 0.0f, 3.0f)),
    _fov(45.0f),
    _nearPlane(0.1f),
    _farPlane(100.0f),
    _currentTextureUnit(0)
{
    if (width && height) {
        _camera.SetPerspective(width, height, _fov, _nearPlane, _farPlane);
        // Essentially, the same 
        _screenSpaceCamera.SetPerspective(width, height, _fov, _nearPlane, _farPlane);
        // _screenSpaceCamera.SetOrthographic(-1.0f, 1.0f, -1.0f, 1.0f,
        //                                     _nearPlane, _farPlane);
    }        
    // for (int i = 0 ; i < 6 ; ++i) {
    //     _shaders[i] = NULL;
    // }
}

FQGLScene::FQGLScene(const char * vertexShader,
                     const char * basicShader,
                     const char * textureShader) :
    _camera(QVector3D(0.0f, 0.0f, 3.0f)),
    _screenSpaceCamera(QVector3D(0.0f, 0.0f, 3.0f))
{
    // We can't set the perspective camera yet, but we can still set
    // the orthographic.
    // _screenSpaceCamera.SetOrthographic(-1.0f, 1.0f, -1.0f, 1.0f,
    //                                     _nearPlane, _farPlane);
    _shaders[0] = vertexShader;
    _shaders[1] = basicShader;
    _shaders[2] = textureShader;
}

FQGLScene::~FQGLScene()
{
}

void
FQGLScene::Release()
{
    if (_basicShader) {
        _basicShader->release();
    }
    if (_textureShader) {
        _textureShader->release();
    }

    std::for_each(_screenSpacePrims.begin(), _screenSpacePrims.end(),
                  [] (FQGLPrimSharedPtr prim) {
                      prim->Release();
                  });

    std::for_each(_prims.begin(), _prims.end(),
                  [] (FQGLPrimSharedPtr prim) {
                      prim->Release();
                  });
}
void
FQGLScene::SetPerspective(int width, int height)
{
    _camera.SetPerspective(width, height, _fov, _nearPlane, _farPlane);
    _screenSpaceCamera.SetPerspective(width, height, _fov, _nearPlane, _farPlane);
    //_camera.SetPerspective(aspect, 45.0f, 0.1f, 100.0f);

    // We need to regenerate the geometry for screen space prims
    _InitPrims();
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

    //QOpenGLShaderProgramSharedPtr basic = InitShaders(_shaders[0], _shaders[1]);
    QOpenGLShaderProgramUniquePtr basic = InitShaders(_shaders[0], _shaders[1]);
    if (basic) {
        _basicShader.swap(basic);
    } else {
        qDebug() << "Unable to link basic shader";
    }

    //QOpenGLShaderProgramSharedPtr tex = InitShaders(_shaders[0], _shaders[2]);
    QOpenGLShaderProgramUniquePtr tex = InitShaders(_shaders[0], _shaders[2]);
    if (tex) {
        tex->bind();
        _textureShader.swap(tex);
        
        _textureShader->setUniformValue("texture1",GL_TEXTURE0);
    } else {
        qDebug() << "Unable to link texture shader";
    }

    // I don't think these need to be bound to anything
    QOpenGLExtraFunctions::glGenQueries(2, _queryIds);

    _InitPrims();
}

void
FQGLScene::AddPrim(const FQGLPrimSharedPtr& prim)
{
    // Sort them now. View type is set on construction, so we don't have to
    // worry about it changing
    if (prim->GetViewType() == FQGLScreenViewType) {
        _screenSpacePrims.push_back(prim);
    } else {
        _prims.push_back(prim);
    }
}

//QOpenGLShaderProgramSharedPtr
const QOpenGLShaderProgramUniquePtr&
FQGLScene::GetShader(bool getTexShader) const
{
    return getTexShader ? _textureShader : _basicShader;
}

void
FQGLScene::Render(const QVector4D& clearColor,
                  FQGLFramebufferType frameBufferType)
{
    // If the prim has a shader, we should get it and set our variables here.
    _SetupShaders(_screenSpaceCamera);
    GLbitfield clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    if (frameBufferType != FQGLDefaultFramebufferType) {
        // Let's clear the stencil too
        clearBits |= GL_STENCIL_BUFFER_BIT;
    }
    glClearColor(clearColor.w(), clearColor.x(), clearColor.x(),
                 clearColor.z());
    glClear(clearBits);

    std::for_each(_screenSpacePrims.cbegin(), _screenSpacePrims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      _RenderPrim(prim, false);
                  });

    // Now, set up the shaders for perspective by using the regular camera
    _SetupShaders(_camera);
    std::for_each(_prims.cbegin(), _prims.cend(),
                  [this, frameBufferType] (const FQGLPrimSharedPtr &prim) {
                      bool doTest = frameBufferType == FQGLPickingFramebufferType;
                      // XXX - this might get tricky if we expect pass or fail
                      // for all prims. But, for now, we don't act on the result,
                      // only primt it out.
                      bool didPass = _RenderPrim(prim, false);
                      if (doTest) {
                          qDebug() << "Passed: " << didPass;
                      }
                  });
}

QVector2D
FQGLScene::GetScreenFromNDC(const QVector3D& ndcPoint) const
{
    FQGLFrustum frustum = _camera.GetFrustum();

    return frustum.ConvertToScreen(_camera.GetUp(),
                                   _camera.GetAspectRatio(), ndcPoint);
}

QVector3D
FQGLScene::GetNDCFromScreen(const QVector2D& screenPoint,
                            const float & depth) const
{
    FQGLFrustum frustum = _camera.GetFrustum();
 
    return frustum.ConvertScreenToFrustum(_camera.GetUp(),
                                          _camera.GetAspectRatio(),
                                          screenPoint, depth);
}

QVector3D
FQGLScene::GetSceneFromCoordinate(const QVector2D& coordinatePoint,
                                  const float & depth) const
{
    FQGLFrustum frustum = _camera.GetFrustum();

    return frustum.ConvertCoordToFrustum(_camera.GetUp(),
                                         _camera.GetAspectRatio(),
                                         coordinatePoint, depth);
}

QVector2D
FQGLScene::GetCoordinateFromScene(const QVector3D& scenePoint) const
{
    FQGLFrustum frustum = _camera.GetFrustum();

    return frustum.ConvertToCoordinate(_camera.GetUp(),
                                       _camera.GetAspectRatio(),
                                       scenePoint);
}

uint
FQGLScene::GetNextTextureBindingUnit()
{
    return _currentTextureUnit++;
}

uint
FQGLScene::GetCurrentTextureBindingUnit() const
{
    return _currentTextureUnit;
}

void
FQGLScene::DeleteTexture(GLuint textureId)
{
    if (glIsTexture(textureId)) {
        glDeleteTextures(1, &textureId);
    }
}

//QOpenGLShaderProgramSharedPtr
QOpenGLShaderProgramUniquePtr
FQGLScene::InitShaders(const char * vshader,
                       const char * fshader)
{
    QOpenGLShaderProgramUniquePtr program =
        std::make_unique<QOpenGLShaderProgram>();
    // QOpenGLShaderProgramSharedPtr program =
    //     std::make_shared<QOpenGLShaderProgram>();
    // QOpenGLShaderProgramSharedPtr program =
    //     new QOpenGLShaderProgram;

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
FQGLScene::_SetupShaders(const FQGLCamera& camera)
{
    _basicShader->bind();
    _basicShader->setUniformValue("view", camera.GetViewMatrix());
    _basicShader->setUniformValue("projection",
                                  camera.GetProjectionMatrix());
    _textureShader->bind();
    _textureShader->setUniformValue("view", camera.GetViewMatrix());
    _textureShader->setUniformValue("projection",
                                    camera.GetProjectionMatrix());
    _textureShader->setUniformValue("texture1", 0);
}

void
FQGLScene::_InitPrims()
{
    std::for_each(_prims.cbegin(), _prims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      prim->Initialize(this);
                  });
    std::for_each(_screenSpacePrims.cbegin(), _screenSpacePrims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      prim->Initialize(this);
                  });
}

bool
FQGLScene::_RenderPrim(const FQGLPrimSharedPtr& prim, bool isTesting,
                       TestType testType)
{
    _PrimRenderState renderState(prim->IsStencil(), this);
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
