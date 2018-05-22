#include "FQGLController.h"

#include "FQGLResponder.h"
#include "FQGLWidget.h"

FQGLController::FQGLController() :
    _responder(NULL)
{
}

FQGLController::~FQGLController()
{

}

void
FQGLController::SetWidget(FQGLWidgetSharedPtr widget)
{
    _widget = widget;
}

void
FQGLController::SetResponder(FQGLResponder * responder)
{
    _responder = responder;
}

void
FQGLController::ReceivedTap(const QVector3D& location) const
{
    if (_responder) {
        _responder->HandleSingleTap(location);
    }
}

