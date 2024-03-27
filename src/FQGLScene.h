#ifndef FQGL_SCENE
#define FQGL_SCENE

#include "FQGLCamera.h"
#include "FQGLTypes.h"

#include <QOpenGLExtraFunctions>
//#include <QOpenGLFunctions>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <vector>


FQGL_DECLARE_PTRS(FQGLPrim);

FQGL_DECLARE_PTRS(QOpenGLShaderProgram)
// class QOpenGLShaderProgram;
// #define QOpenGLShaderProgramSharedPtr QOpenGLShaderProgram*

// Describes a scene. has a camera and prims. This is driven by the
// FQGLwidget. It's the 3D world, not windowing
class FQGLScene : protected QOpenGLExtraFunctions
{
public:
    FQGLScene(int width = 0, int height = 0);

    FQGLScene(const char * vertexShader,
              const char * basicShader,
              const char * textureShader);

    virtual ~FQGLScene();

    virtual void Release();
    void SetPerspective(int width, int height);

    void SetShaders(const char * vertexShader,
                    const char * basicShader,
                    const char * textureShader);
    
    void Initialize();

    void AddPrim(const FQGLPrimSharedPtr& prim);

    const QOpenGLShaderProgramUniquePtr& GetShader(bool getTexShader=true) const;

    void Render(const QVector4D& clearColor,
                FQGLFramebufferType frameBufferType);

    // For interactions, we provide screen <-> space conversions. This can also
    // be used for texture points (with an extra conversion).
    QVector2D GetScreenFromNDC(const QVector3D& ndcPoint) const;

    // For a screen point in 2D NDC space, it will return a 3D point in the
    // camera frustum. If -1.0f is passed in as the depth, it will use the
    // near plane.
    QVector3D GetNDCFromScreen(const QVector2D& screenPoint,
                                    const float & depth) const;

    QVector3D GetSceneFromCoordinate(const QVector2D& coordinatePoint,
                                     const float & depth) const;

    QVector2D GetCoordinateFromScene(const QVector3D& scenePoint) const;

    // static QOpenGLShaderProgramSharedPtr InitShaders(const char * vertexShader,
    //                                                  const char * fragmentShader);
    static QOpenGLShaderProgramUniquePtr InitShaders(const char * vertexShader,
                                                     const char * fragmentShade);

    // Increments and returns the next binding unit. Not reentrant
    uint GetNextTextureBindingUnit();

    uint GetCurrentTextureBindingUnit() const;

    void DeleteTexture(GLuint textureId);

protected:
    void _SetupShaders(const FQGLCamera& camera);
    
    void _InitPrims();

    // Helper class which pushes stuff on the render stack when we render a
    // prim, and resets is when we're done rendering the prim.
    class _PrimRenderState {
    public:
        _PrimRenderState(bool isStencil, QOpenGLFunctions* functions);
        ~_PrimRenderState();
    private:
        bool _isStencil;
        QOpenGLFunctions* _functions;
    };

    friend _PrimRenderState;

    enum TestType {
        AllSamplePassedTestType,
        AnySamplePassedTestType,
    };
    bool _RenderPrim(const FQGLPrimSharedPtr& prim, bool isTesting=false,
                     TestType testType=AllSamplePassedTestType);
    
private:
    // QOpenGLShaderProgramSharedPtr _basicShader;
    // QOpenGLShaderProgramSharedPtr _textureShader;
    QOpenGLShaderProgramUniquePtr _basicShader;
    QOpenGLShaderProgramUniquePtr _textureShader;
    FQGLCamera _camera;
    FQGLCamera _screenSpaceCamera;

    float _fov;
    float _nearPlane;
    float _farPlane;
    uint _currentTextureUnit;
    std::vector<FQGLPrimSharedPtr> _prims;
    std::vector<FQGLPrimSharedPtr> _screenSpacePrims;

    const char * _shaders[3];

    // this may switch to more tests at some point, but for now, array of 1.
    GLuint _queryIds[2];
};

#endif
