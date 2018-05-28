#include "FQGLController.h"

#include "FQGLResponder.h"
#include "FQGLWidget.h"

FQGLController::FQGLController() :
    //    _widget(NULL),
    _responder(NULL)
{
}

FQGLController::~FQGLController()
{

}

void
FQGLController::SetWidget(const FQGLWidgetPtr& widget)
{
    _widget = widget;
}

void
FQGLController::SetResponder(FQGLResponder * responder)
{
    _responder = responder;
}

void
FQGLController::ReceivedTap(const QVector2D& location, TapType tapType) const
{
    if (_responder) {
        if ((tapType == DoubleTapType) || (tapType == RightTapType)) {
            _responder->HandleRightTap(location);
        } else {
            _responder->HandleSingleTap(location);
        }
    }
}

void
FQGLController::RenderComplete()
{
    if (_responder) {
        _responder->OnRenderComplete();
    }
}
