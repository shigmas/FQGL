#ifndef FQGL_SCENE
#define FQGL_SCENE


#include "FQGLTypes.h"

#include <QOpenGLFunctions>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <vector>

FQGL_DECLARE_PTRS(FQGLCamera);
FQGL_DECLARE_PTRS(FQGLPrim);

FQGL_DECLARE_PTRS(QOpenGLShaderProgram)

// Describes a scene. has a camera and prims. This is driven by the
// FQGLwidget. It's the 3D world, not windowing
class FQGLScene : protected QOpenGLFunctions
{
public:
    FQGLScene(int width = 0, int height = 0);

    FQGLScene(const char * vertexShader,
              const char * basicShader,
              const char * textureShader);

    virtual ~FQGLScene();

    void SetPerspective(int width, int height);

    void SetShaders(const char * vertexShader,
                    const char * basicShader,
                    const char * textureShader);
    
    void Initialize();

    void AddPrim(const FQGLPrimSharedPtr& prim, bool asStencilPrim=false);

    QOpenGLShaderProgramSharedPtr GetShader(bool basicShader=true) const;

    void Render(const QVector4D& clearColor,
                FQGLFramebufferType frameBufferType);

    // For interactions, we provide an API to get points in the screen.
    QVector3D GetCameraScreenPoint(const QVector2D& screenPoint) const;

    static QOpenGLShaderProgram *InitShaders(const char * vertexShader,
                                             const char * fragmentShader);


protected:
    enum TestType {
        AllSamplePassedTestType,
        AnySamplePassedTestType,
    };
    void _InitPrims();

    bool _RenderPrim(const FQGLPrimSharedPtr& prim, bool isTesting=false,
                     TestType testType=AllSamplePassedTestType);
    
private:
    QOpenGLShaderProgramSharedPtr _basicShader;
    QOpenGLShaderProgramSharedPtr _textureShader;
    FQGLCameraSharedPtr _camera;

    std::vector<FQGLPrimSharedPtr> _prims;
    std::vector<FQGLPrimSharedPtr> _stencilPrims;

    const char * _shaders[3];

    // this may switch to more tests at some point, but for now, array of 1.
    GLuint _queryIds[2];
};

#endif
