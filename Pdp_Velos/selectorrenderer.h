#ifndef SELECTORRENDERER_H
#define SELECTORRENDERER_H

#include "renderer.h"

namespace bss {
class SelectorRenderer;
}

class SelectorRenderer : public Renderer
{
public:
    SelectorRenderer();
    ~SelectorRenderer();

    virtual void draw() override;
    void updateData(const QVector<float> &data);
    void prepareData(unsigned int verticesCount, unsigned int dataCount);
};

#endif // SELECTORRENDERER_H
