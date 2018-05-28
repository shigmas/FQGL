#include "FQGLPrim.h"


class CubePrim : public FQGLPrim
{
public:
    CubePrim(float length, const QVector4D& color);
    virtual ~CubePrim();

protected:
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) override;

    virtual void _CreateIndices(GLuint **indices,
                                uint &numIndices) override;

    GLenum _GetDrawMode() const override;

private:
    float _length;
    QVector4D _color;
};
