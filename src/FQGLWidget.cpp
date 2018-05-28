#include "FQGLWidget.h"

#include "FQGLController.h"
#include "FQGLScene.h"

#include <QOpenGLFramebufferObject>
#include <QGestureEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QDebug>

#include <algorithm>

FQGLWidget::FQGLWidget(FQGLControllerPtr controller,
                       const QVector4D& clearColor, QWidget *parent) :
    QOpenGLWidget(parent),
    _controller(controller),
    _scene(new FQGLScene),
    _clearColor(clearColor),
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
FQGLWidget::SetSceneShaders(const char * vertexShader,
                            const char * basicShader,
                            const char * textureShader)
{
    _scene->SetShaders(vertexShader, basicShader, textureShader);
}

void
FQGLWidget::SetScenePerspective(int width, int height)
{
    _scene->SetPerspective(width, height);
}

void
FQGLWidget::AddPrimToScene(const FQGLPrimSharedPtr& prim,
                           bool asStencilPrim)
{
    _scene->AddPrim(prim, asStencilPrim);
}

bool
FQGLWidget::EnableTextureBuffer()
{
    // enabling an enabled buffer does nothing
    if (!_framebuffers[FQGLTextureFramebufferType].first) {
        // We might want to just grab part of the framebuffer image, so enable
        // the depth and stencils. In the future, we can add parameters to the
        // this function.
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setTextureTarget(GL_TEXTURE_2D);

        // Not sure why we need to be twice the size to get the whole screen.
        QOpenGLFramebufferObjectSharedPtr fb = QOpenGLFramebufferObjectSharedPtr
            (new QOpenGLFramebufferObject(2*size(), format));

        // Creating the buffer binds it, so bind back to the default.
        fb->bindDefault();
        _framebuffers[FQGLTextureFramebufferType].first = fb;
    }
    _framebuffers[FQGLTextureFramebufferType].second = true;
    return _framebuffers[FQGLTextureFramebufferType].first->isValid();
}

bool
FQGLWidget::IsTextureBufferEnabled() const
{
    return _framebuffers[FQGLTextureFramebufferType].second;
}

void
FQGLWidget::DisableTextureBuffer()
{
    // Free up the framebuffer.
    _framebuffers[FQGLTextureFramebufferType].second = false;
}

bool
FQGLWidget::EnablePickTestingBuffer()
{
    // enabling an enabled buffer does nothing
    if (!_framebuffers[FQGLPickingFramebufferType].first) {
        // We might want to just grab part of the framebuffer image, so enable
        // the depth and stencils. In the future, we can add parameters to the
        // this function.
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

        QOpenGLFramebufferObjectSharedPtr fb = QOpenGLFramebufferObjectSharedPtr
            (new QOpenGLFramebufferObject(size(), format));
        glEnable(GL_STENCIL_TEST);
        // XXX - do this later. This makes the whole window go black
        // // Passes if ( ref & mask ) != ( stencil & mask ).
        //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // What to set the stencil buffer value to if:
        // Stencil fails, stencil passes/depth fails, stencil passes/depth passes
        // (or is not enabled)
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
        fb->bindDefault();
        _framebuffers[FQGLPickingFramebufferType].first = fb;

    }
    _framebuffers[FQGLPickingFramebufferType].second = true;
    return _framebuffers[FQGLPickingFramebufferType].first->isValid();
}

bool
FQGLWidget::IsPickTestingBufferEnabled() const
{
    return _framebuffers[FQGLPickingFramebufferType].second;
}
    
void
FQGLWidget::DisablePickTestingBuffer()
{
    // Free up the framebuffer.
    _framebuffers[FQGLPickingFramebufferType].second = false;
}

GLuint
FQGLWidget::GetTextureIdFromLastRender()
{
    // If we don't have a valid texture, return the max. (We address by index,
    // so max is invalid.)
    GLuint texId = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
    if (_framebuffers[FQGLTextureFramebufferType].first) {
        texId = _framebuffers[FQGLTextureFramebufferType].first->texture();
    }

    return texId;
}


QVector3D
FQGLWidget::ToScenePoint(const QVector2D& ndcPoint) const
{
    return _scene->GetCameraScreenPoint(ndcPoint);
}

QVector2D
FQGLWidget::ToTexPoint(const QVector2D& ndcPoint) const
{
    return QVector2D((ndcPoint.x() + 1.0)/2.0f, (ndcPoint.y() + 1.0)/2.0f);
}

void
FQGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);

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
    for (int i = 0 ; i < FQGL_NUM_FRAMEBUFFERS ; ++i) {
        if (_framebuffers[i].second) {
            _framebuffers[i].first->bind();
            _scene->Render(_clearColor, (FQGLFramebufferType) i);
            if (i == FQGLTextureFramebufferType) {
                _framebuffers[i].first->toImage().save("foo.png");
            }
        }
    }
    
    QOpenGLFramebufferObject::bindDefault();
    _scene->Render(_clearColor, FQGLDefaultFramebufferType);

    if (FQGLControllerSharedPtr ctrlr = FQGLControllerSharedPtr(_controller)) {
        ctrlr->RenderComplete();
    }
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
    FQGLController::TapType tapType;
    switch (event->button()) {
    case Qt::LeftButton:
        tapType = FQGLController::LeftTapType;
        break;
    case Qt::RightButton:
        tapType = FQGLController::RightTapType;
        break;
    default:
        tapType = FQGLController::LeftTapType;
    }
    
    QVector2D ndcPoint = _ToNDC(event->x(), event->y());
    // Just testing the flow for now. This needs to get translated from the
    // screen to the camera.
    if (FQGLControllerSharedPtr ctrlr = FQGLControllerSharedPtr(_controller)) {
        ctrlr->ReceivedTap(ndcPoint, tapType);
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
