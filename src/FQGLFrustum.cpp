#include "FQGLFrustum.h"

#include <QtMath>
#include <QVector2D>

FQGLFrustum::FQGLFrustum(const float& width, const float& height,
                         const float& fovAngle, const float& nearPlane,
                         const float& farPlane, const QVector3D& position,
                         QVector3D& lookAt, const QVector3D& up) :
    FQGLFrustum(width, height, nearPlane, farPlane, position, lookAt)
{
    _fovAngle = fovAngle;
    QVector3D dirNormal = _GetDirectionNormal(_position, _lookAt);
    QVector3D rightNormal = _GetRightNormal(up, dirNormal);
    QVector3D upNormal = up.normalized();
    float aspect = _width/_height;
    float halfHeight = _GetHeightFromDistance(_nearPlane, _fovAngle);
    QVector3D center = _position + (dirNormal *_nearPlane);

    _SetPlanePoints(&_points[NearTopRightPointIndex], rightNormal, upNormal, 
                    center, halfHeight*width, halfHeight);

    halfHeight = _GetHeightFromDistance(_farPlane, _fovAngle);
    center = _position + (dirNormal * _farPlane);
        
    _SetPlanePoints(&_points[FarTopRightPointIndex], rightNormal, upNormal,
                    center, halfHeight*aspect, halfHeight);

    _BuildPlanesFromPoints();
    _frustumType = PerspectiveFrustumType;
}

FQGLFrustum::FQGLFrustum(const float& width, const float& height,
                         const float& nearPlane, const float& farPlane,
                         const QVector3D position, const QVector3D& lookAt,
                         const QVector3D& up) :
    FQGLFrustum(width, height, nearPlane, farPlane, position, lookAt)
{
    QVector3D dirNormal = _GetDirectionNormal(position, lookAt);
    QVector3D rightNormal = _GetRightNormal(up, dirNormal);
    QVector3D upNormal = up.normalized();

    // Set the four points at the near plane
    QVector3D center = position + (dirNormal * _nearPlane);        
    _SetPlanePoints(&_points[NearTopRightPointIndex], rightNormal, upNormal,
                    center, width*.5f, height*.5f);

    // Set the four points at the far plane
    center = position + (dirNormal * _farPlane);
    _SetPlanePoints(&_points[FarTopRightPointIndex], rightNormal, upNormal,
                    center, width*.5f, height*.5f);
        
    _BuildPlanesFromPoints();
    _frustumType = OrthographicFrustumType;
}

FQGLFrustum::FQGLFrustum(const float& width, const float& height,
                         const float& nearPlane, const float& farPlane,
                         const QVector3D& position, const QVector3D& lookAt) :
    _position(position),
    _lookAt(lookAt),
    _nearPlane(nearPlane),
    _farPlane(farPlane),
    _width(width),
    _height(height)
{}

FQGLFrustum::FQGLFrustum(const float& width, const float& height,
                         const float& fovAngle, const float& nearPlane,
                         const float& farPlane, const QVector3D& position,
                         const QVector3D& lookAt, const QVector3D& up,
                         const QVector2D& pickPoint, const float& pickSize) :
    FQGLFrustum(width, height, nearPlane, farPlane, position, lookAt)
{
    float aspect = width/height;
    QVector3D fwNormal = _GetDirectionNormal(_position, _lookAt);
    QVector3D rightNormal = _GetRightNormal(up, fwNormal);
    QVector3D upNormal = up.normalized();

    // So, grab the four points with the two normals, and the other data
    // for the near place
    _SetPlanePoints(&_points[NearTopRightPointIndex], _position, _fovAngle,
                    aspect, fwNormal, upNormal,rightNormal, nearPlane,
                    pickPoint, pickSize);
    _SetPlanePoints(&_points[FarTopRightPointIndex], _position, fovAngle,
                    aspect, fwNormal, upNormal, rightNormal,
                    farPlane*.90f, pickPoint, pickSize);

    _BuildPlanesFromPoints();
    _frustumType = PickFrustumType;
}

FQGLFrustum::FQGLFrustum(const float& width, const float& height,
                         const float& nearPlane, const float& farPlane,
                         const QVector3D& position, const QVector3D& lookAt,
                         const QVector3D& up, const QVector2D& pickPoint,
                         const float& pickSize) :
    FQGLFrustum(width, height, nearPlane, farPlane, position, lookAt)   
{
    const float& widthOff = width*0.5f;
    float heightOff = height*0.5f;

    // All the same for the frustum we're picking in .
    QVector3D fwNormal = _GetDirectionNormal(position,lookAt);
    QVector3D rightNormal = _GetRightNormal(up, fwNormal);
    QVector3D upNormal = up.normalized();

    // Convert NDC to our camera "units"
    QVector3D rightPick = rightNormal * (widthOff*pickPoint.x());
    QVector3D upPick = upNormal * (heightOff*pickPoint.y());
        
    QVector3D center = _position + (fwNormal *_nearPlane);
    QVector3D frustCenter = center + (rightPick + upPick);
        
    _SetPlanePoints(&_points[NearTopRightPointIndex], rightNormal,
                    upNormal, frustCenter, pickSize*0.5f, pickSize*0.5f);
    center = _position + (fwNormal * _farPlane);
    frustCenter = center + (rightPick + upPick);
    _SetPlanePoints(&_points[FarTopRightPointIndex], rightNormal,
                    upNormal, frustCenter, pickSize*0.5f, pickSize*0.5f);

    _BuildPlanesFromPoints();
    _frustumType = PickFrustumType;
}

FQGLFrustum::FQGLFrustum(QVector3D * points, FQGLPlane *planes,
                         FrustumType frustumType)
{
    memcpy(_points, points, sizeof(QVector3D)*8);
    memcpy(_planes, planes, sizeof(FQGLPlane)*6);
    _frustumType = frustumType;
}

QVector3D
FQGLFrustum::GetPointInFrustum(const QVector3D& cameraUp,
                               const QVector2D& screenPoint,
                               const float& aspect,
                               const float& distance) const
{
    QVector3D fwNormal = _GetDirectionNormal(_position, _lookAt);
    QVector3D rightNormal = _GetRightNormal(cameraUp, fwNormal);
    QVector3D upNormal = cameraUp.normalized();

    float zDist = distance != -1.0f ? distance : (_position - _lookAt).length();
    // Width and height at the distance.
    float height = _GetHeightFromDistance(zDist, _fovAngle);
    float width = height * aspect;

    QVector3D point = _position + (fwNormal*zDist);
    point += (rightNormal * screenPoint.x()*width);
    point += (upNormal * screenPoint.y()*height);

    return point;
}

// If we already have a frustum, we can get a slice of that.
FQGLFrustum
FQGLFrustum::GetPickFrustum(const QVector2D& pickPoint, const float& pickSize)
{
    QVector3D points[8];
    QVector3D fwNormal = _GetDirectionNormal(_position, _lookAt);
    // The near and far planes are parallel, so we can use the same up normal
    QVector3D upNormal = (_points[TopLeftPlaneIndex] - 
                          _points[BottomLeftPlaneIndex]).normalized();
    // The right pointing normal.  Same for near and far.
    QVector3D rightNormal = (_points[TopRightPlaneIndex] -
                             _points[TopLeftPlaneIndex]).normalized();
    float aspect = _width/_height;
    
    if (_frustumType == PerspectiveFrustumType) {
        // So, grab the four points with the two normals, and the other
        // data for the near place
        _SetPlanePoints(&points[TopRightPlaneIndex], _position, _fovAngle,
                        aspect, fwNormal, upNormal, rightNormal,
                        _nearPlane, pickPoint, pickSize);
        _SetPlanePoints(&points[TopRightPlaneIndex], _position, _fovAngle,
                        aspect, fwNormal, upNormal, rightNormal,
                        _farPlane*.90f, pickPoint, pickSize);
    } else if (_frustumType == OrthographicFrustumType) {
        float widthOff = _width*0.5f;
        float heightOff = _height*0.5f;
        QVector3D rightPick = rightNormal * (widthOff*pickPoint.x());
        QVector3D upPick = upNormal * (heightOff*pickPoint.y());
        
        // Set the four points at the near plane
        QVector3D center = _position + (fwNormal * _nearPlane);
        QVector3D frustCenter = center + (rightPick + upPick);
        _SetPlanePoints(&points[NearTopRightPointIndex], rightNormal, upNormal,
                        frustCenter, pickSize*.5f, pickSize*.5f);
        // Set the four points at the far plane
        center = _position + (fwNormal * _farPlane);
        frustCenter = center + (rightPick + upPick);
        _SetPlanePoints(&points[FarTopRightPointIndex], rightNormal, upNormal,
                        frustCenter, pickSize*.5f, pickSize*.5f);
    }
    _BuildPlanesFromPoints();
    return FQGLFrustum(_points,_planes, PickFrustumType);
}

// Testing for pick
bool
FQGLFrustum::IsPointInFrustum(const QVector3D& point,
                              const QMatrix4x4& transform) const
{
    bool result = true;
    QVector3D transformedPoint = transform * point;
    for (int i = 0 ; i < 6 ; ++i) {
        FQGLPlane plane = _planes[i];
        float distance = plane.DistanceFromPlane(transformedPoint);
        if (distance > 0) {
            // we're outside the plane.
            result = false;
            //NSLog(@"Outside: %g", distance);
            break;
        }
    }
    
    // Went through all planes and we're inside.
    return result;
}

// Protected


bool
FQGLFrustum::IsFaceInFrustum(const std::vector<QVector3D>& face,
                             const QMatrix4x4& transform) const
{
    int inFrust;
    int outFrust;
    int inPlanes = 0;
    bool intersect = false;

    for (int i = 0 ; i < 6 ; ++i) {
        inFrust = 0;
        outFrust = 0;
        
        FQGLPlane plane = _planes[i];
        // for each vertex in the face do ...
        // get out of the cycle as soon as a box as corners
        // both inside and out of the frustum
        for (unsigned long k = 0;
             k < face.size() && (inFrust==0 || outFrust==0);
             k++) {
            QVector3D point = face[k];
            point = transform * point;
            float distance = plane.DistanceFromPlane(point);
            if (distance < 0) {
                outFrust++;
            } else {
                inFrust++;
            }
        }
        if (inFrust == 0) {
            // everything was on one side (outside) of this plane.  so
            // definitely outside the frustum
            return false;
        } else if (outFrust) {
            // point was inside at least one plane, so there's some
            // some intersection
            intersect = true;
        } else {
            inPlanes++;
        }
    }

    // There was *some* intersection with more than one plane.  So, we'll
    // just say that there *was* an intersection.  There's a case where this
    // isn't true, but we'll let it go for now.
    return intersect or (inPlanes == 6);
}

float
FQGLFrustum::_GetHeightFromDistance(const float& near, const float& fov) const
{
    float angle = qDegreesToRadians(fov)/2.0f;

    return qTan(angle) * near;
}

QVector3D
FQGLFrustum::_GetDirectionNormal(const QVector3D& position,
                                 const QVector3D& lookAt) const
{
    QVector3D dirVector = lookAt - position;
    return dirVector.normalized();
}

QVector3D
FQGLFrustum::_GetRightNormal(const QVector3D& up,
                             const QVector3D& dirNormal) const
{
    return QVector3D::crossProduct(dirNormal, up).normalized();

}

void
FQGLFrustum::_SetPlanePoints(QVector3D* facePoints,
                             const QVector3D& rightNormal,
                             const QVector3D& upNormal, const QVector3D& center,
                             const float& width, const float& height)
{
    QVector3D upVector = upNormal * height;
    QVector3D rightVector = rightNormal * width;

    QVector3D topCenter = center + upVector;
    facePoints[0] = topCenter + rightVector;
    facePoints[1] = topCenter - rightVector;
    QVector3D bottomCenter = center - upVector;
    facePoints[2] = bottomCenter + rightVector;
    facePoints[3] = bottomCenter - rightVector;
}

void
FQGLFrustum::_SetPlanePoints(QVector3D* facePoints,
                             const QVector3D& position,
                             const float& fovAngle, const float& aspect, 
                             const QVector3D& forwardNormal,
                             const QVector3D&upNormal,
                             const QVector3D& rightNormal,
                             const float& distance,
                             const QVector2D& pickPoint,
                             const float& pickSize)
{
    // Get the actual height and width
    float height = _GetHeightFromDistance(distance, fovAngle);
    float width = height*aspect;

    // Get our real hxwxd w/ the normals
    QVector3D dirVec = position + (forwardNormal * distance);
    QVector3D upVec = upNormal * height;
    QVector3D rightVec = rightNormal * width;

    facePoints[0] = _GetPointFrom(dirVec, upVec, rightVec,
                                  pickPoint.x() + pickSize,
                                  pickPoint.y() + pickSize);
    facePoints[1] = _GetPointFrom(dirVec, upVec, rightVec,
                                  pickPoint.x() - pickSize,
                                  pickPoint.y() + pickSize);
    facePoints[2] = _GetPointFrom(dirVec, upVec, rightVec,
                                  pickPoint.x() + pickSize,
                                  pickPoint.y() - pickSize);
    facePoints[3] = _GetPointFrom(dirVec, upVec, rightVec,
                                  pickPoint.x() - pickSize,
                                  pickPoint.y() - pickSize);
}

QVector3D
FQGLFrustum::_GetPointFrom(const QVector3D& forward, const QVector3D& up,
                           const QVector3D& right, const float& width,
                           const float& height)
{
    QVector3D pickUp = up * height;
    QVector3D pickWidth = right * width;
    
    return forward + (pickUp + pickWidth);
}

void
FQGLFrustum::_BuildPlanesFromPoints()
{
    // Shortcut to build near and far planes:
    _planes[NearPlaneIndex] = FQGLPlane(_points[NearTopRightPointIndex],
                                        _points[NearTopLeftPointIndex],
                                        _points[NearBottomRightPointIndex]);
    _planes[FarPlaneIndex] = FQGLPlane(_points[NearBottomLeftPointIndex],
                                       _points[FarTopLeftPointIndex],
                                       _points[FarBottomRightPointIndex]);
    _planes[TopPlaneIndex] = FQGLPlane(_points[FarTopLeftPointIndex],
                                       _points[NearTopLeftPointIndex],
                                       _points[FarTopRightPointIndex]);
    _planes[BottomPlaneIndex] = FQGLPlane(_points[FarBottomRightPointIndex],
                                          _points[NearBottomRightPointIndex],
                                          _points[FarBottomLeftPointIndex]);
    _planes[LeftPlaneIndex] = FQGLPlane(_points[FarBottomLeftPointIndex],
                                        _points[NearBottomLeftPointIndex],
                                        _points[FarTopLeftPointIndex]);
    _planes[RightPlaneIndex] = FQGLPlane(_points[FarBottomRightPointIndex],
                                         _points[FarTopRightPointIndex],
                                         _points[NearBottomRightPointIndex]);
}
