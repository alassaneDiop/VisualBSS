#ifndef SELECTORRENDERER_H
#define SELECTORRENDERER_H

#include "renderer.h"

namespace bss {
class SelectorRenderer;
}

class QRectF;
class SelectorRenderer : public Renderer
{
public:
    SelectorRenderer();
    ~SelectorRenderer();

    virtual void draw() override;
    void updateData(const QRectF &data);
};

#endif // SELECTORRENDERER_H
