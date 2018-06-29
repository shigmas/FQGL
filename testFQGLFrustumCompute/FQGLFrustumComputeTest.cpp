#include "FQGLFrustumComputeTest.h"

#include "FQGLFrustumCompute.h"

#include <QtGlobal>

#include <math.h>

FQGLFrustumComputeTest::FQGLFrustumComputeTest() :
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
FQGLFrustumComputeTest::initTestCase()
{
    _compute = FQGLFrustumComputeSharedPtr(new FQGLFrustumCompute(_position,
                                                                  _lookAt, _up,
                                                                  _width/_height,
                                                                  _fov));
}

void
FQGLFrustumComputeTest::testPerspectiveScreenPoint()
{
    QVector3D testPt(0.5f, -0.5f, 0.0f);
    QVector2D screen = _compute->GetScreenPointFromFrustum(testPt);
    QVector3D testPtPrime = _compute->GetFrustumPointFromScreen(screen, 3.0f);
    
    double radius = 0.5f;
    double l = sqrt(pow(radius, 2.0)*2);
    testPt = QVector3D(l, l, 0.0f);
    qDebug() << "orig: " << testPt;
    screen = _compute->GetScreenPointFromFrustum(testPt);
    qDebug() << "screen: " << screen;

    // Convert it back to the original
    testPtPrime = _compute->GetFrustumPointFromScreen(screen, 3.0f);
    qDebug() << "orig: " << testPtPrime;

    // Try negative
    testPt = QVector3D(-l, -l, 0.0f);
    screen = _compute->GetScreenPointFromFrustum(testPt);
    qDebug() << "screen: " << screen;

}

void
FQGLFrustumComputeTest::testPerspectiveEquivalentPoint()
{
    double radius = 0.5f;
    double l = sqrt(pow(radius, 2.0)*2);
    QVector3D testPt(l, l, 0.0f);
    QVector3D equiv = _compute->ConvertPointToDepth(testPt, 0.1f);
    qDebug() << "equiv: " << equiv;
}


// We can't put this in the header because it'll appear twice, so, if we have
// a cpp, it goes here
QTEST_APPLESS_MAIN(FQGLFrustumComputeTest)

// We only want this if it's in the header.
//#include "testFQGLFrustumCompute.moc"
