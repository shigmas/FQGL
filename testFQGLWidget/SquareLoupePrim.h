#include "FQGLPrim.h"

// The loupe exists as a screen space prim. It magnifies the scene behind it
// but isn't actually part of the scene. And, while this is an implementation
// detail, we render the visible part of the screen as a texture, which has to
// be in screen coordinates.
//
// Screen space prims are in the (-1,-1) to (1,1) space. They'll get translated
// to be visible in 3D space.
class SquareLoupePrim : public FQGLPrim
{
public:
    SquareLoupePrim(float length, float magnification,
                    const QVector4D& color);
    virtual ~SquareLoupePrim();

protected:
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) override;

    virtual void _CreateIndices(GLuint **indices,
                                uint &numIndices) override;

    virtual GLenum _GetDrawMode(FQGLPrim::_PrimMode primMode) const override;

private:
    float _length;
    float _magnification;
    QVector4D _color;
};
