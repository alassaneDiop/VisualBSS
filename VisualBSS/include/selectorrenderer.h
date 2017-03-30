#ifndef SELECTORRENDERER_H
#define SELECTORRENDERER_H

#include "renderer.h"


namespace bss {
class SelectorRenderer;
}

class QRectF;

/**
 * @brief The SelectorRenderer class Class that draw selector.
 */
class SelectorRenderer : public Renderer
{
public:
    SelectorRenderer();
    ~SelectorRenderer();

    /**
     * @brief draw Draw selector.
     */
    virtual void draw() override;

    /**
     * @brief updateData Update new vertices position of selector.
     * @param data Vertices to send to the graphics card.
     */
    void updateData(const QRectF &data);
};

#endif // SELECTORRENDERER_H
