#ifndef TRIPRENDERER_H
#define TRIPRENDERER_H

#include "renderer.h"


namespace bss {
class TripRenderer;
}

/**
 * @brief The TripRenderer class Class dedicated to draw trips which are
 * represented by lines primitives.
 */
class TripRenderer : public Renderer
{
public:
    TripRenderer();
    virtual ~TripRenderer();

    /**
     * @brief draw Redefinition of draw function, draws all lines that have been
     * sent with <i>Renderer::sendData(const QVector<float>& data, unsigned int verticesCount)</i>.
     */
    virtual void draw() override;
};

#endif // TRIPRENDERER_H
