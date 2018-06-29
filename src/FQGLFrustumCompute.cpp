#include "FQGLFrustumCompute.h"

#include <QtMath>

FQGLFrustumCompute::FQGLFrustumCompute(const QVector3D & position,
                                       const QVector3D & lookAt,
                                       const QVector3D & up,
                                       const float & aspectRatio,
                                       const float & fov) :
    _position(position),
    _dirNormal(_GetDirectionNormal(position, lookAt)),
    _rightNormal(_GetRightNormal(up, _dirNormal)),
    _upNormal(up.normalized()),
    _aspect(aspectRatio),
    _fov(fov)
{
}

QVector2D
FQGLFrustumCompute::GetScreenPointFromFrustum(const QVector3D& point) const
{
    QVector2D coord = GetCoordPointFromFrustum(point);
    // Convert from (0,1 to (-1,1)
    return QVector2D((.5 * coord.x() + 0.5),
                     (.5 * coord.y() + 0.5));
}

QVector2D
FQGLFrustumCompute::GetCoordPointFromFrustum(const QVector3D& point) const
{
    // Break down our vector into components, relative to our compute
    // parameters
    QVector3D ptParts = _GetComponents(point);

    //  .5x + 0.5, then scale
    // Convert from model space to -1 to 1
    float height = _GetHeightFromDistance(ptParts.z());
    float width = height * _aspect;

    // normalize
    return QVector2D(ptParts.x() / width, ptParts.y() / height);
}

QVector3D
FQGLFrustumCompute::GetFrustumPointFromScreen(const QVector2D& screen,
                                              const float & depth) const
{
    // Convert from (0,1) to (-1,1) space: 2(x - 0.5), then use the coord
    // function
    return GetFrustumPointFromCoord(QVector2D(2.0f*screen.x() - 1.0f,
                                              2.0f*screen.y() - 1.0f),
                                    depth);
}

QVector3D
FQGLFrustumCompute::GetFrustumPointFromCoord(const QVector2D& coord,
                                             const float & depth) const
{
    float height = _GetHeightFromDistance(depth);
    float width = height * _aspect;

    // normalize to model, and piece by piece, add the different vectors to
    // our position
    return _BuildFromComponents(coord.x() * width, coord.y() * height, depth);
}

QVector3D
FQGLFrustumCompute::ConvertPointToDepth(const QVector3D& point,
                                        const float & depth) const
{
    // Break down our vector into components, relative to our compute
    // parameters
    QVector3D ptParts = _GetComponents(point);

    // This is NDC, so we want the max X that is w/in the frustum
    float heightAtPoint = _GetHeightFromDistance(ptParts.z());

    // NDC, so these are ratios already.
    float heightRatio = ptParts.y() / heightAtPoint;
    float widthRatio = ptParts.x() / (heightAtPoint * _aspect);

    float heightAtDepth = _GetHeightFromDistance(depth);
    float widthAtDepth = heightAtDepth * _aspect;

    // Now, apply these as ratios to get the resulting point. We have depth to
    // use to apply it to height and width.
    return  _BuildFromComponents(widthRatio * widthAtDepth,
                                 heightRatio * heightAtDepth, depth);
}

QVector3D
FQGLFrustumCompute::_GetDirectionNormal(const QVector3D& position,
                                        const QVector3D& lookAt) const
{
    QVector3D dirVector = lookAt - position;
    return dirVector.normalized();
}

QVector3D
FQGLFrustumCompute::_GetRightNormal(const QVector3D& up,
                                    const QVector3D& dirNormal) const
{
    return QVector3D::crossProduct(dirNormal, up).normalized();

}

float
FQGLFrustumCompute::_GetHeightFromDistance(const float& dist) const
{
    // from tan(alpha) = opp/adj;
    return qTan(qDegreesToRadians(_fov/2.0)) * dist;
}

QVector3D
FQGLFrustumCompute::_GetComponents(const QVector3D& point) const
{
    QVector3D relativePoint = point - _position;
    return QVector3D(QVector3D::dotProduct(relativePoint, _rightNormal),
                     QVector3D::dotProduct(relativePoint, _upNormal),
                     QVector3D::dotProduct(relativePoint, _dirNormal));
}

QVector3D
FQGLFrustumCompute::_BuildFromComponents(const float& width,
                                         const float& height,
                                         const float& depth) const
{
    return _position + width * _rightNormal
        + height * _upNormal
        + depth * _dirNormal;
}
