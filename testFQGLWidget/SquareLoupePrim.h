#include "FQGLPrim.h"

// This has to be a screen space prim. But it's not, for now.
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
