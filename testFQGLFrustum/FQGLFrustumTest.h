#ifndef FQGL_FRUSTUM_TEST
#define FQGL_FRUSTUM_TEST

#include <QtTest>

#include "FQGLFrustum.h"
#include "FQGLTypes.h"

FQGL_DECLARE_PTRS(FQGLFrustum);

class FQGLFrustumTest : public QObject
{
    Q_OBJECT

public:
    FQGLFrustumTest();

private Q_SLOTS:
    void initTestCase();
    void testPerspectiveFrustum();

private:
    bool _CloseCompare(const float& f1, const float& f2);
    
private:
    float _width;
    float _height;
    float _fov;
    float _nearPlane;
    float _farPlane;
    QVector3D _position;
    QVector3D _lookAt;
    QVector3D _up;

    FQGLFrustumSharedPtr _frustum;
};


#endif // FQGL_FRUSTUM_TEST
