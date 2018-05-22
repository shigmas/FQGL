#ifndef FQGL_WIDGET
#define FQGL_WIDGET


#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QVector2D>

#include "FQGLTypes.h"

FQGL_DECLARE_PTRS(FQGLController);
FQGL_DECLARE_PTRS(FQGLScene);

class QGestureEvent;

class FQGLWidget : public QOpenGLWidget,
                   protected QOpenGLFunctions
{
    Q_OBJECT

    typedef FQGLWidget Self;
    typedef QOpenGLWidget Parent;
public:

    FQGLWidget(FQGLControllerPtr controller, QWidget *parent = 0);
    virtual ~FQGLWidget();

    void SetScene(FQGLSceneSharedPtr scene);
    FQGLSceneSharedPtr GetScene() const;
                  
protected:
    // QOpenGLWidget and QWidget overrides
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual bool event(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    void _ConfigureStandardFramebuffer();
    void _ConfigureSamplingFramebuffer();

    void _EnableGestures();

    // We don't have an override for this, so we catch it in the main event
    // handler and pass it on to here.
    bool _HandleGesture(QGestureEvent *event);

    QVector2D _ToNDC(const int& x, const int& y) const;
    
private:
    FQGLControllerPtr _controller;
    FQGLSceneSharedPtr _scene;

    // These are in screen coordinates. They get converted to NDC when we pass it
    // to the scene.
    int _lastX;
    int _lastY;

    GLuint _queryIds[2];
};

#endif
