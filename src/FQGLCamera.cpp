
#include "FQGLCamera.h"

#include <QtMath>

FQGLCamera::FQGLCamera(const QVector3D& position, const QVector3D& lookAt,
                       const QVector3D& up) :
        _position(position),
        _lookAt(lookAt),
        _up(up)
{
}

FQGLCamera::~FQGLCamera()
{

}

FQGLFrustum
FQGLCamera::GetFrustum() const
{
    QVector3D position = GetPosition();
    QVector3D lookAt = GetLookAt();
    QVector3D up = GetUp();
    if (_projType == PerspectiveProjectionType) {
        float width;
        float height;
        if (_perspProps.aspectRatio == -1.0) {
            width = _perspProps.width;
            height = _perspProps.height;
        } else {
            width = 2.0;
            height = 2.0;
        }
        return FQGLFrustum(width, height,
                           _perspProps.fov, _perspProps.nearPlane,
                           _perspProps.farPlane, position, lookAt, up);
    } else { // OrthographicProjectionType
        return FQGLFrustum(_perspProps.width, _perspProps.height,
                           _perspProps.fov, _perspProps.nearPlane,
                           _perspProps.farPlane, position, lookAt, up);
        // XXX BROKEN
        // return FQGLFrustum(_orthoProps.width, _orthoProps.height,
        //                    _orthoProps.nearPlane, _orthoProps.farPlane,
        //                    GetPosition(), GetLookAt(), GetUp());
    }
}

QVector3D
FQGLCamera::GetPosition() const
{
    return _position;
}

QVector3D
FQGLCamera::GetLookAt() const
{
    return _lookAt;
}

QVector3D
FQGLCamera::GetUp() const
{
    return _up;
}

float
FQGLCamera::GetAspectRatio() const
{
    if (_projType == PerspectiveProjectionType) {
        if (_perspProps.aspectRatio == -1.0) {
            return fabs(_perspProps.width/_perspProps.height);
        } else {
            return _perspProps.aspectRatio;
        }
    } else { // OrthographicProjectionType
        return fabs((_orthoProps.right - _orthoProps.left) /
                    (_orthoProps.top - _orthoProps.bottom));
    }
}

QMatrix4x4
FQGLCamera::SetPerspective(float width, float height, float fov,
                           float nearPlane, float farPlane)
{
    _perspProps = _PerspectiveProps(width, height, fov, nearPlane, farPlane);
    _projection.setToIdentity();

    return SetToPerspective();
}

QMatrix4x4
FQGLCamera::SetPerspective(float aspectRatio, float fov,
                           float nearPlane, float farPlane)
{
    _perspProps = _PerspectiveProps(aspectRatio, fov, nearPlane, farPlane);
    _projection.setToIdentity();

    return SetToPerspective();
}

QMatrix4x4
FQGLCamera::SetToPerspective()
{
    _projection.setToIdentity();
    if (_perspProps.initialized) {
        if (_perspProps.aspectRatio == -1.0f) {
            float aspect = fabs(_perspProps.width/_perspProps.height);
            _projection.perspective(_perspProps.fov, aspect,
                                    _perspProps.nearPlane,
                                    _perspProps.farPlane);
        } else {
            _projection.perspective(_perspProps.fov, _perspProps.aspectRatio,
                                    _perspProps.nearPlane,
                                    _perspProps.farPlane);
        }
        _projType = PerspectiveProjectionType;
    }

    return _projection;
}

QMatrix4x4
FQGLCamera::SetOrthographic(float left, float right, float bottom,
                            float top, float nearPlane, float farPlane)
{
    _orthoProps = _OrthographicProps(left, right, bottom, top,
                                     nearPlane, farPlane);

    return SetToOrthographic();
}

QMatrix4x4
FQGLCamera::SetToOrthographic()
{
    _projection.setToIdentity();
    if (_orthoProps.initialized) {
        _projection.ortho(_orthoProps.left, _orthoProps.right,
                          _orthoProps.bottom, _orthoProps.top,
                          _orthoProps.nearPlane, _orthoProps.farPlane);
        _projType = OrthographicProjectionType;
    }

    return _projection;
}

QMatrix4x4
FQGLCamera::GetProjectionMatrix() const
{
    return _projection;
}

QMatrix4x4
FQGLCamera::GetViewMatrix() const
{
    QMatrix4x4 camMatrix;
    camMatrix.lookAt(_position, _lookAt, _up);

    return camMatrix;
}

void
FQGLCamera::_CreateGeometry(FQGLPrimVertex **vertices,
                            uint &numVertices)
{
    Q_UNUSED(vertices);
    Q_UNUSED(numVertices);
    // We actually have no geometry to create.
}

GLenum
FQGLCamera::_GetDrawMode() const
{
    return GL_TRIANGLES;
}
