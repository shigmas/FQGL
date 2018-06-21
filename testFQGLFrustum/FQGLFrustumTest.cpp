#include "FQGLFrustumTest.h"

#include <QtGlobal>

#include <math.h>

FQGLFrustumTest::FQGLFrustumTest() :
    _width(800.0f),
    _height(600.0f),
    _fov(45.0f),
    _nearPlane(0.1f),
    _farPlane(100.0f),
    _position(QVector3D(0.0f, 0.0f, 3.0f)),
    _lookAt(QVector3D(0.0f, 0.0f, 0.0f)),
    _up(QVector3D(0.0f, 1.0f, 0.0f))
{

}

void
FQGLFrustumTest::initTestCase()
{
    _frustum = FQGLFrustumSharedPtr(new FQGLFrustum(_width, _height,
                                                    _fov, _nearPlane, _farPlane,
                                                    _position, _lookAt, _up));
}

void
FQGLFrustumTest::testPerspectiveFrustum()
{
    QVector3D points[8];

    _frustum->GetFrustumPoints(points);

    // Camera is position
    QVector3D center(0.0f, 0.0f, 2.0f);
    QVector3D converted = _frustum->ConvertFrustumPoint(_up, _width/_height,
                                                        center, 0.1f);
    qDebug() << "center screen: " << converted;
    QVERIFY2(center.x() == converted.x(), "X doesn't match");
    QVERIFY2(center.y() == converted.y(), "Y doesn't match");
    QVERIFY2(_CloseCompare(converted.z(), _position.z() - _nearPlane),
             "Z doesn't match near plane");

    // More typical, some object at the origin
    double radius = 0.5f;
    double l = sqrt(pow(radius, 2.0)*2);
    QVector3D testPt(l, l, 0.0f);
    converted = _frustum->ConvertFrustumPoint(_up, _width/_height, testPt,
                                              0.0f);
    qDebug() << "test point: " << converted;

    // QVector2D screen = _frustum->ConvertScreenToFrustum(_up, testPt,
    //                                                     _width/_height);

    //qDebug() << "screen: " << screen;
}

bool
FQGLFrustumTest::_CloseCompare(const float& f1, const float& f2)
{
    qDebug() << "compare: " << f1 << ", " << f2;
    return abs(f1 - f2) < 0.001;
    
}


// We can't put this in the header because it'll appear twice, so, if we have
// a cpp, it goes here
QTEST_APPLESS_MAIN(FQGLFrustumTest)

// We only want this if it's in the header.
//#include "testFQGLFrustum.moc"
