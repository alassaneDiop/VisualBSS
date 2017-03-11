#ifndef SELECTORRENDERER_H
#define SELECTORRENDERER_H

#include "renderer.h"

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
