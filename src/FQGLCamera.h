#ifndef FQGL_CAMERA
#define FQGL_CAMERA

#include "FQGLCamera.h"
#include "FQGLFrustum.h"
#include "FQGLPrim.h"

#include <QVector3D>
#include <QMatrix4x4>

const float FQGL_DefaultYaw         = -90.0f;
const float FQGL_DefaultPitch       = 0.0f;
const float FQGL_DefaultSpeed       = 2.5f;
const float FQGL_DefaultSensitivity = 0.1f;
const float FQGL_DefaultZoom        = 45.0f;

// A specialized prim. It can be visible with geometry, but the main point is
// that it exists in a scene, so it's a prim.
class FQGLCamera : FQGLPrim
{
public:
    enum ProjectionType {
        PerspectiveProjectionType,
        OrthographicProjectionType,
    };

    // Creates a camera located at \p position, pointing at \p lookAt, with
    // up being \p up.
    FQGLCamera(const QVector3D& position,
               const QVector3D& lookAt = QVector3D(0.0f, 0.0f, 0.0f),
               const QVector3D& up = QVector3D(0.0f, 1.0f, 0.0f));

    virtual ~FQGLCamera();

    // Uses our current projection (orth or perspective) to create and return
    // the camera frustum.
    FQGLFrustum GetFrustum() const;

    QVector3D GetPosition() const;

    QVector3D GetLookAt() const;

    QVector3D GetUp() const;

    float GetAspectRatio() const;

    // The first time we set to perspective, we need to provide the parameters.
    // Afterwards, we can just SetToPerspective.
    QMatrix4x4 SetPerspective(float width, float height, float fov,
                              float nearPlane, float farPlane);

    // If we're in NDC, we don't need width and height. but we need an
    // aspect ratio.
    QMatrix4x4 SetPerspective(float aspectRatio, float fov,
                              float nearPlane, float farPlane);

    // If the parameters have been set, this will return the perspective
    // matrix. Otherwise, it will return identity.
    QMatrix4x4 SetToPerspective();

    // The first time we set to orthographic, we need to provide the parameters.
    // Afterwards, we can just SetToOrthographic.
    QMatrix4x4 SetOrthographic(float left, float right, float bottom,
                               float top, float nearPlane, float farPlane);

    // If the parameters have been set, this will return the orthographic
    // matrix. Otherwise, it will return identity.
    QMatrix4x4 SetToOrthographic();

    QMatrix4x4 GetProjectionMatrix() const;

    QMatrix4x4 GetViewMatrix() const;

protected:
    // We currently can't draw this prim, so this function is a stub.
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) override;    

    virtual GLenum _GetDrawMode() const override;

    struct _PerspectiveProps
    {
        _PerspectiveProps() :
            initialized(false) {}

        _PerspectiveProps(float width, float height, float fov,
                          float nearPlane, float farPlane) :
            initialized(true),
            aspectRatio(-1.0f),
            width(width),
            height(height),
            fov(fov),
            nearPlane(nearPlane),
            farPlane(farPlane) {}

        _PerspectiveProps(float aspectRatio, float fov,
                          float nearPlane, float farPlane) :
            initialized(true),
            aspectRatio(aspectRatio),
            width(-1.0f),
            height(-1.0f),
            fov(fov),
            nearPlane(nearPlane),
            farPlane(farPlane) {}

        bool initialized;
        float aspectRatio;
        float width;
        float height;
        float fov;
        float nearPlane;
        float farPlane;
    };

    struct _OrthographicProps
    {
        _OrthographicProps() :
            initialized(false) {}

        _OrthographicProps(float left, float right, float bottom, float top,
                           float nearPlane, float farPlane) :
            initialized(true),
            left(left),
            right(right),
            bottom(bottom),
            top(top),
            nearPlane(nearPlane),
            farPlane(farPlane) {}

        bool initialized;
        float left;
        float right;
        float bottom;
        float top;
        float nearPlane;
        float farPlane;
    };

private:
    QVector3D _position;
    QVector3D _lookAt;
    QVector3D _up;

    QMatrix4x4 _projection;

    ProjectionType _projType;
    _PerspectiveProps _perspProps;
    _OrthographicProps _orthoProps;
};

#endif
