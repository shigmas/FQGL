#ifndef FQGL_CONTROLLER
#define FQGL_CONTROLLER

#include "FQGLTypes.h"

#include <QVector3D>

FQGL_DECLARE_PTRS(FQGLResponder);
FQGL_DECLARE_PTRS(FQGLWidget);

// Owns the GL widget, and is held by the GL widget.
class FQGLController
{
public:
    FQGLController();
    virtual ~FQGLController();

    void SetWidget(FQGLWidgetSharedPtr widget);

    void SetResponder(FQGLResponder * responder);

    void ReceivedTap(const QVector3D& location) const;
    
protected:

private:
    FQGLWidgetSharedPtr _widget;
    FQGLResponder* _responder;
};

#endif // FQGL_CONTROLLER
