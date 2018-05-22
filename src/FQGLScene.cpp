#include "FQGLScene.h"

#include "FQGLCamera.h"
#include "FQGLFrustum.h"
#include "FQGLPrim.h"

#include <QOpenGLShaderProgram>

#include <QtMath>

#include <algorithm>

FQGLScene::FQGLScene(int width, int height) :
    _camera(new FQGLCamera(QVector3D(0.0f, 0.0f, 3.0f)))
{
    if (width && height) {
        _camera->SetPerspective(width, height, 45.0f, 0.1f, 100.0f);
    }        
    for (int i = 0 ; i < 6 ; ++i) {
        _shaders[i] = NULL;
    }
}

FQGLScene::FQGLScene(const char * vertexShader,
                     const char * fragmentShader,
                     const char * geomShader,
                     const char * tessControlShader,
                     const char * tessEvalShader,
                     const char * computeShader) :
    _camera(new FQGLCamera(QVector3D(0.0f, 0.0f, 3.0f)))
{
    _shaders[0] = vertexShader;
    _shaders[1] = fragmentShader;
    _shaders[2] = geomShader;
    _shaders[3] = tessControlShader;
    _shaders[4] = tessEvalShader;
    _shaders[5] = computeShader;
}

FQGLScene::~FQGLScene()
{
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
                      const char * fragmentShader)
{
    _shaders[0] = vertexShader;
    _shaders[1] = fragmentShader;
}

void
FQGLScene::Initialize()
{
    QOpenGLShaderProgram * program = _InitShaders(_shaders[0], _shaders[1],
                                                  _shaders[2],  _shaders[3],
                                                  _shaders[4],  _shaders[5]);
    if (program) {
        _sceneShader = QOpenGLShaderProgramSharedPtr(program);
    } else {
        qDebug() << "Unable to link shader program";
    }

    _InitPrims();
}

void
FQGLScene::AddPrim(const FQGLPrimSharedPtr& prim)
{
    _prims.push_back(prim);
}

void
FQGLScene::Render()
{
    _sceneShader->bind();
    _sceneShader->setUniformValue("view", _camera->GetViewMatrix());
    _sceneShader->setUniformValue("projection",
                                  _camera->GetProjectionMatrix());

    std::for_each(_prims.cbegin(), _prims.cend(),
                  [this] (const FQGLPrimSharedPtr &prim) {
                      prim->Render(_sceneShader);
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
FQGLScene::_InitShaders(const char * vshader,
                        const char * fshader,
                        const char * geomShader,
                        const char * tessControlShader,
                        const char * tessEvalShader,
                        const char * computeShader)
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

    if (geomShader) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::Geometry,
                                              geomShader)) {
            qDebug() << "fshader failed";
            return NULL;
        }
    }

    if (tessControlShader) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::TessellationControl,
                                              tessControlShader)) {
            qDebug() << "tessControl shader failed";
            return NULL;
        }
    }

    if (tessEvalShader) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation,
                                              tessEvalShader)) {
            qDebug() << "tessEval fshader failed";
            return NULL;
        }
    }

    if (computeShader) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::Compute,
                                              computeShader)) {
            qDebug() << "compute  failed";
            return NULL;
        }
    }

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
                      prim->Initialize(_sceneShader);
                  });
}
