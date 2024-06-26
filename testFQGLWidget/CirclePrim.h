#include "FQGLPrim.h"


class LoupePrim : public FQGLPrim
{
public:
    LoupePrim(float radius, int numSamples, float magnification,
              const QVector4D& color);
    ~LoupePrim();

protected:
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) override;

    GLenum _GetDrawMode() const override;

private:
    float _radius;
    int _numSamples;
    float _magnification;
    QVector4D _color;
};
