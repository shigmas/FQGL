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
                const QVector3D& position, const QVector3D& lookAt,
                const QVector3D& up);

    // Orthographic constructor
    FQGLFrustum(const float& width, const float& height, const float& nearPlane,
                const float& farPlan, const QVector3D position,
                const QVector3D& lookAt, const QVector3D& up);

    // If we already have a frustum, we can get a slice of that.
    FQGLFrustum GetPickFrustum(const QVector2D& pickPoint,
                               const float& pickSize);

    // These two functions are very similar, but different enough (I think) to
    // have two functions. As the API gets fleshed out, we might start putting
    // the math into a helper.

    // Returns a point in the frustum along the vector formed from the camera
    // location to the \p point in the plane formed at \p depth along the
    // camera location to the lookAt point. In short, a point which would be
    // identically rendered given the projection. Default depth is 0.1 (the
    // near plane).
    QVector3D ConvertFrustumPoint(const QVector3D& cameraUp,
                                  const float& aspect,
                                  const QVector3D& point,
                                  const float& depth = 0.1f) const;

    QVector2D ConvertToScreen(const QVector3D& cameraUp,
                              const float& aspect,
                              const QVector3D& point) const;

    // Takes an screen point and returns its equivalent point in the
    // frustum's near plane. screen point is (0-1, 0-1)
    // Note: To be visible, the return value may have to be slightly extended
    // to be slightly inside the frustum.
    QVector3D ConvertScreenToFrustum(const QVector3D& cameraUp,
                                     const float& aspect,
                                     const QVector2D& screenPoint,
                                     const float & depth) const;
    
    // Testing for pick
    bool IsPointInFrustum(const QVector3D& point,
                          const QMatrix4x4& transform = QMatrix4x4()) const;
    bool IsFaceInFrustum(const std::vector<QVector3D>& face,
                         const QMatrix4x4& transform = QMatrix4x4()) const;

    // Fills a pre-allocated array of points. Uses the indices in the enum
    void GetFrustumPoints(QVector3D* points) const;

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

    // Returns the height (y) of the frustum at the distance. (This does half
    // the height of the frustum, since it doesn't include negative height
    float _GetHeightFromDistance(const float& dist, const float& fov) const;

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
