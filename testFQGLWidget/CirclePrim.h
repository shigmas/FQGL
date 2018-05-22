#include "FQGLPrim.h"


class CirclePrim : public FQGLPrim
{
public:
    CirclePrim(float radius, int numSamples, const QVector4D& color);
    ~CirclePrim();

protected:
    virtual void _CreateGeometry(FQGLPrimVertex **vertices,
                                 uint &numVertices) override;

private:
    float _radius;
    int _numSamples;
    QVector4D _color;
};
