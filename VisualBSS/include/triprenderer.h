#ifndef TRIPRENDERER_H
#define TRIPRENDERER_H

#include "renderer.h"


namespace bss {
class TripRenderer;
}

class TripRenderer : public Renderer
{
public:
    TripRenderer();
    virtual ~TripRenderer();
    virtual void draw() override;
};

#endif // TRIPRENDERER_H
