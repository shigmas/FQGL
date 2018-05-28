#include "FQGLPrim.h"

// This has to be a screen space prim. But it's not, for now.
class LoupePrim : public FQGLPrim
{
public:
    LoupePrim(float radius, int numSamples, float magnification,
              const QVector4D& color);
    virtual ~LoupePrim();

protected:
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) override;

    virtual GLenum _GetDrawMode() const override;

private:
    float _radius;
    int _numSamples;
    float _magnification;
    QVector4D _color;
};
