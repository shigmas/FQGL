#ifndef FQGL_SCENE
#define FQGL_SCENE


#include "FQGLTypes.h"

#include <QVector2D>
#include <QVector3D>

#include <vector>

FQGL_DECLARE_PTRS(FQGLCamera);
FQGL_DECLARE_PTRS(FQGLPrim);

FQGL_DECLARE_PTRS(QOpenGLShaderProgram)


// Describes a scene. has a camera and prims. This is driven by the
// FQGLwidget.
class FQGLScene
{
public:
    FQGLScene(int width = 0, int height = 0);

    FQGLScene(const char * vertexShader,
              const char * fragmentShader,
              const char * geomShader = NULL,
              const char * TessControlShader = NULL,
              const char * TessEvalShader = NULL,
              const char * computeShader = NULL);

    virtual ~FQGLScene();

    void SetPerspective(int width, int height);

    void SetShaders(const char * vertexShader,
                    const char * fragmentShader);
    
    void Initialize();

    void AddPrim(const FQGLPrimSharedPtr& prim);

    void Render();

    // For interactions, we provide an API to get points in the screen.
    QVector3D GetCameraScreenPoint(const QVector2D& screenPoint) const;
    
protected:
    QOpenGLShaderProgram * _InitShaders(const char * vertexShader,
                                        const char * fragmentShader,
                                        const char * geomShader = NULL,
                                        const char * tessControlShader = NULL,
                                        const char * tessEvalShader = NULL,
                                        const char * computeShader = NULL);

    void _InitPrims();

private:
    QOpenGLShaderProgramSharedPtr _sceneShader;
    FQGLCameraSharedPtr _camera;

    std::vector<FQGLPrimSharedPtr> _prims;
    const char * _shaders[6];
};

#endif
