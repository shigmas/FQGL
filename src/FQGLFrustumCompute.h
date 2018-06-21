#ifndef FQGL_FRUSTUMCOMPUTE
#define FQGL_FRUSTUMCOMPUTE

#include <QVector2D>
#include <QVector3D>

// Object used to compute parts of the frustum. It's helpful not to have to
// construct things over and over again.
class FQGLFrustumCompute
{
public:
    FQGLFrustumCompute(const QVector3D & position,
                       const QVector3D & lookAt,
                       const QVector3D & up,
                       const float & aspectRatio,
                       const float & fov);

    // Gets the screen point for the given point. (0,1)
    QVector2D GetScreenPoint(const QVector3D& point) const;

    QVector3D GetFrustumPoint(const QVector2D& point,
                              const float & depth) const;

    // Returns the equivalent point in the frustum at the given depth. (Note:
    // In the compute, we don't know the near and far planes, so this may still
    // be outside of the frustum).
    QVector3D ConvertPointToDepth(const QVector3D& point,
                                  const float & depth) const;

protected:
    QVector3D _GetDirectionNormal(const QVector3D& position,
                                  const QVector3D& lookAt) const;

    QVector3D _GetRightNormal(const QVector3D& up,
                              const QVector3D& dirNormal) const;

    // Returns the TOTAL view height. So, this is above and below the eye
    float _GetHeightFromDistance(const float& dist) const;

    // Returns the vector, relative to the camera. We return it as a vector
    // to make the API easier. It might be more ... intuitive if it was a
    // triple.
    QVector3D _GetComponents(const QVector3D& point) const;

    // Returns 
    QVector3D _BuildFromComponents(const float& height, const float& width,
                                   const float& depth) const;

    private:
    QVector3D _position;
    QVector3D _dirNormal;
    QVector3D _rightNormal;
    QVector3D _upNormal;
    float _aspect;
    float _fov;
};

#endif // FQGL_FRUSTUMCOMPUTE

