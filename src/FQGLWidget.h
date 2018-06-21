#ifndef FQGL_WIDGET
#define FQGL_WIDGET


#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <utility>
#include <vector>

#include "FQGLTypes.h"

FQGL_DECLARE_PTRS(FQGLController);
FQGL_DECLARE_PTRS(FQGLPrim);
FQGL_DECLARE_PTRS(FQGLScene);
FQGL_DECLARE_PTRS(QOpenGLFramebufferObject);

class QGestureEvent;

// Handles windowing and interactions. Drives OpenGL.
class FQGLWidget : public QOpenGLWidget,
                   protected QOpenGLFunctions
{
    Q_OBJECT

    typedef FQGLWidget Self;
    typedef QOpenGLWidget Parent;
public:
    FQGLWidget(FQGLControllerPtr controller,
               const QVector4D& clearColor = QVector4D(1.0f, 1.0f, 1.0f, 1.0f),
               QWidget *parent = 0);
    virtual ~FQGLWidget();

    void SetSceneShaders(const char * vertexShader,
                         const char * basicShader,
                         const char * textureShader);

    void SetScenePerspective(int width, int height);

    void AddPrimToScene(const FQGLPrimSharedPtr& primt);

    bool EnableTextureBuffer();

    bool IsTextureBufferEnabled() const;

    void DisableTextureBuffer();

    bool EnablePickTestingBuffer();

    bool IsPickTestingBufferEnabled() const;

    void DisablePickTestingBuffer();

    GLuint GetTextureIdFromLastRender();

    // Converts the screen point to a point in the scene. By default, it will
    // be the near plane.
    QVector3D ToScenePoint(const QVector2D& screenPoint,
                           const float& depth=-1.0f) const;
    
    // For want of a better name, coordinate is the (-1,-1) to (1,1)
    QVector2D ScreenToCoordinates(const QVector2D& screenPoint,
                                  bool invertY = false) const;

    
protected:
    // QOpenGLWidget and QWidget overrides
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual bool event(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    void _EnableGestures();

    // We don't have an override for this, so we catch it in the main event
    // handler and pass it on to here.
    bool _HandleGesture(QGestureEvent *event);

    QVector2D _ToNDC(const int& x, const int& y) const;

    QVector2D _ToScreen(const int& x, const int& y) const;

private:
    FQGLControllerPtr _controller;
    FQGLSceneSharedPtr _scene;
    QVector4D _clearColor;

    // We could use the same framebuffer for the 2 existing cases, but we could
    // have different prims for each, so keeping them separate. Possibly, we
    // could do a combined.
    std::pair<QOpenGLFramebufferObjectSharedPtr,
              bool> _framebuffers[FQGL_NUM_FRAMEBUFFERS];

    // These are in screen coordinates. They get converted to NDC when we pass it
    // to the scene.
    int _lastX;
    int _lastY;
};

#endif
