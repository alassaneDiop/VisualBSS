#ifndef STATIONRENDER_H
#define STATIONRENDER_H

#include "renderer.h"

namespace bss {
class StationRenderer;
}

class StationRenderer : public Renderer
{
public:
    StationRenderer();
    virtual ~StationRenderer();
    virtual void draw() override;

    void sendData(const QVector<float>& data, unsigned int verticesCount);
    void drawFilledCircle(unsigned int triangleAmount, unsigned int radius);

};

#endif // STATIONRENDER_H
