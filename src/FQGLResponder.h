#ifndef FQGL_RESPONDER
#define FQGL_RESPONDER

#include <QVector2D>

// Pure virtual class which responds to user interaction
class FQGLResponder
{
public:
    FQGLResponder();
    virtual ~FQGLResponder();

    virtual void HandleSingleTap(const QVector2D& location) = 0;
    virtual void HandleRightTap(const QVector2D& location) = 0;

    virtual void HandleDrag(const QVector2D& location) = 0;

    virtual void OnPreRenderComplete() = 0;
    virtual void OnRenderComplete() = 0;
protected:
};

#endif // FQGL_RESPONDER
