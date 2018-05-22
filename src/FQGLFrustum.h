#ifndef FQGL_FRUSTUM
#define FQGL_FRUSTUM

#include "FQGLPlane.h"

#include <QMatrix4x4>
#include <QVector3D>

#include <vector>

class FQGLFrustum
{
public:
    enum PointIndex {
        NearTopRightPointIndex = 0,
        NearTopLeftPointIndex,
        NearBottomRightPointIndex,
        NearBottomLeftPointIndex,
        FarTopRightPointIndex,
        FarTopLeftPointIndex,
        FarBottomRightPointIndex,
        FarBottomLeftPointIndex,
    };

    enum PlaneIndex {
        NearPlaneIndex = 0,
        FarPlaneIndex,
        TopPlaneIndex,
        BottomPlaneIndex,
        LeftPlaneIndex,
        RightPlaneIndex,
    };

    enum FrustumType {
        PerspectiveFrustumType,
        OrthographicFrustumType,
        PickFrustumType,
    };

    enum PlanePointIndex {
        TopRightPlaneIndex = 0,
        TopLeftPlaneIndex,
        BottomRightPlaneIndex,
        BottomLeftPlaneIndex,
    };

    // Perspective constructor
    FQGLFrustum(const float& width, const float& height, const float& fovAngle,
                const float& nearPlane, const float& farPlane,
                const QVector3D& position, QVector3D& lookAt,
                const QVector3D& up);

    // Orthographic constructor
    FQGLFrustum(const float& width, const float& height, const float& nearPlane,
                const float& farPlan, const QVector3D position,
                const QVector3D& lookAt, const QVector3D& up);

    // If we already have a frustum, we can get a slice of that.
    FQGLFrustum GetPickFrustum(const QVector2D& pickPoint,
                               const float& pickSize);

    // We do this quite a bit internally, from the internal points, but this
    // takes coordinates in the frustum coordinate. The default distance is the
    // near plane.
    QVector3D GetPointInFrustum(const QVector3D& cameraUp,
                                const QVector2D& screenPoint,
                                const float& aspect,
                                const float& distance = -1.0f) const;
    
    // Testing for pick
    bool IsPointInFrustum(const QVector3D& point,
                          const QMatrix4x4& transform = QMatrix4x4()) const;
    bool IsFaceInFrustum(const std::vector<QVector3D>& face,
                         const QMatrix4x4& transform = QMatrix4x4()) const;

protected:
    // Helper constructor for the other constructors
    FQGLFrustum(const float& width, const float& height, const float& nearPlane,
                const float& farPlane, const QVector3D& position,
                const QVector3D& lookAt);

    // Pick Frustum is slice of space in the camera viewing area.  This version
    // creates it in the perspective projection.  We could create it from an
    // existing frustum, but we also want to be able to create it from scratch
    FQGLFrustum(const float& width, const float& height, const float& fovAngle,
                const float& nearDistance, const float& farDistance,
                const QVector3D& position, const QVector3D& lookAt,
                const QVector3D& up, const QVector2D& pickPoint,
                const float& pickSize);

    FQGLFrustum(const float& width, const float& height, const float& nearPlane,
                const float& farPlane, const QVector3D& position,
                const QVector3D& lookAt, const QVector3D& up,
                const QVector2D& pickPoint, const float& pickSize);

    FQGLFrustum(QVector3D * points, FQGLPlane *planes,
                FrustumType frustumType);

    float _GetHeightFromDistance(const float& near, const float& fov) const;

    QVector3D _GetDirectionNormal(const QVector3D& position,
                                  const QVector3D& lookAt) const;

    QVector3D _GetRightNormal(const QVector3D& up,
                              const QVector3D& dirNormal) const;

    void _SetPlanePoints(QVector3D* facePoints,
                         const QVector3D& rightNormal,
                         const QVector3D& upNormal,
                         const QVector3D& center, const float& width,
                         const float& height);

    void _SetPlanePoints(QVector3D* pointsArray,
                         const QVector3D& position,
                         const float& fovAngle, const float& aspect, 
                         const QVector3D& forwardNormal,
                         const QVector3D&upNormal, const QVector3D& rightNormal,
                         const float& distance, const QVector2D& pickPoint,
                         const float& pickSize);

    QVector3D _GetPointFrom(const QVector3D& forward, const QVector3D& up,
                            const QVector3D& right, const float& width,
                            const float& height);

private:
    void _BuildPlanesFromPoints();

private:
    FrustumType _frustumType;

    QVector3D _position;
    QVector3D _lookAt;
    float _nearPlane;
    float _farPlane;
    float _fovAngle;
    float _width;
    float _height;
    
    QVector3D _points[8];
    FQGLPlane _planes[6];
};
#endif // FQGL_FRUSTUM
