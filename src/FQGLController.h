#ifndef FQGL_CONTROLLER
#define FQGL_CONTROLLER

#include "FQGLTypes.h"

#include <QVector2D>

FQGL_DECLARE_PTRS(FQGLResponder);
FQGL_DECLARE_PTRS(FQGLWidget);

// Owns the GL widget, and is held by the GL widget.
class FQGLController
{
public:
    enum TapType {
        SingleTapType,
        DoubleTapType,
        LeftTapType,
        RightTapType,
    };

    FQGLController();
    virtual ~FQGLController();

    void SetWidget(const FQGLWidgetPtr& widget);

    void SetResponder(FQGLResponder * responder);

    void ReceivedTap(const QVector2D& location, TapType tapType) const;

    void PreRenderComplete();

    void RenderComplete();

protected:

private:
    FQGLWidgetPtr _widget;
    FQGLResponder* _responder;
};

#endif // FQGL_CONTROLLER
