#include "FQGLWidget.h"

#include "FQGLController.h"
#include "FQGLScene.h"

#include <QOpenGLFramebufferObject>
#include <QGestureEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QDebug>

#include <algorithm>

FQGLWidget::FQGLWidget(FQGLControllerPtr controller, QWidget *parent) :
    QOpenGLWidget(parent),
    _controller(controller),
    _scene(new FQGLScene),
    _lastX(-1),
    _lastY(-1)

{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);

    // Just one bit is enough for now.
    format.setStencilBufferSize(8);
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        // OpenGL 4+ on Desktop:
        qDebug() << "OpenGL 4.1";
        format.setVersion(4, 1);
    } else {
        // OpenGL ES
        qDebug() << "OpenGL ES 3.0";
        format.setVersion(3, 0);

        // barely related, but if we're ES, we're on mobile, so turn on
        // watching for gestures.
        _EnableGestures();
    }
    // We don't need such backwards compatability.
    format.setProfile(QSurfaceFormat::CoreProfile); 
    // On OSX, we're supposed to do this before we show the window. Or, at
    // least the default format.
    QSurfaceFormat::setDefaultFormat(format);
    setFormat(format);

}

FQGLWidget::~FQGLWidget()
{
}

void
FQGLWidget::SetScene(FQGLSceneSharedPtr scene)
{
    _scene = scene;
}

FQGLSceneSharedPtr
FQGLWidget::GetScene() const
{
    return _scene;
}

void
FQGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // For other framebuffers, we set things like the stencil.

    _scene->Initialize();
}

void
FQGLWidget::resizeGL(int w, int h)
{
    _scene->SetPerspective(w, h);
    update();
}

void::
FQGLWidget::paintGL()
{
    QOpenGLFramebufferObject::bindDefault();
    _scene->Render();
}

bool
FQGLWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent = dynamic_cast<QGestureEvent *>(event);
        if (gestureEvent) {
            return _HandleGesture(gestureEvent);
        }
    }

    return Parent::event(event);
}

void
FQGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
        break;
    case Qt::Key_S:
        break;
    case Qt::Key_A:
        break;
    case Qt::Key_D:
        break;
    default:
        // do nothing
        qDebug() << "Skipping key";
    }
    update();
}

void
FQGLWidget::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Press (NDC): (" << x << ", " << y << ")";
    QVector2D ndcPoint = _ToNDC(event->x(), event->y());
    // Just testing the flow for now. This needs to get translated from the
    // screen to the camera.
    if (FQGLControllerSharedPtr ctrlr = FQGLControllerSharedPtr(_controller)) {
        QVector3D scenePoint = _scene->GetCameraScreenPoint(ndcPoint);
        //QVector3D scenePoint = QVector3D(ndcPoint.x(), ndcPoint.y(), 0);
        ctrlr->ReceivedTap(scenePoint);
    }

    update();
}

void
FQGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "Mouse: (" << event->x() << ", " << event->y() << ")";
    update();
}

void
FQGLWidget::_ConfigureStandardFramebuffer()
{
    // All our default OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void
FQGLWidget::_ConfigureSamplingFramebuffer()
{
    glEnable(GL_DEPTH_TEST);
    // LESS is default anyway
    glDepthFunc(GL_LESS);

    // Not sure....
    glClearStencil(0);

    glEnable(GL_STENCIL_TEST);
    // // Passes if ( ref & mask ) != ( stencil & mask ).
    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // What to set the stencil buffer value to if:
    // Stencil fails, stencil passes/depth fails, stencil passes/depth passes
    // (or is not enabled)
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    glGenQueries(2, _queryIds);
}

void
FQGLWidget::_EnableGestures()
{
    grabGesture(Qt::TapGesture);
    grabGesture(Qt::PinchGesture);
        // | Qt::TapAndHoldGesture |
        //         Qt::PanGesture | Qt::PinchGesture | Qt::SwipeGesture);
}

bool
FQGLWidget::_HandleGesture(QGestureEvent *event)
{
    const QList<QGesture*> gestures = event->gestures();

    std::for_each(gestures.cbegin(), gestures.cend(),
                  [] (const QGesture *gesture) {
                      if (gesture->gestureType() == Qt::TapGesture) {
                          qDebug() << "gesture: tap";
                      } else if (gesture->gestureType() == Qt::PinchGesture) {
                          qDebug() << "gesture: pinch";
                      } else {
                          qDebug() << "Unhandled gesture";
                      }
                  });

    return true;
}

QVector2D
FQGLWidget::_ToNDC(const int& x, const int& y) const
{
    float mid = (float)width()/2.0;
    float ndcX = (x - mid)/mid;
    mid = (float)height()/2.0;
    float ndcY = (-y + mid)/mid;

    return QVector2D(ndcX, ndcY);
}
