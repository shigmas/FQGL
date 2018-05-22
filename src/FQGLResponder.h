#ifndef FQGL_RESPONDER
#define FQGL_RESPONDER

#include <QVector3D>

// Pure virtual class which responds to user interaction
class FQGLResponder
{
public:
    FQGLResponder();
    virtual ~FQGLResponder();

    virtual void HandleSingleTap(const QVector3D& location) = 0;

protected:
};

#endif // FQGL_RESPONDER
