#ifndef GLYPHRENDERER_H
#define GLYPHRENDERER_H

#include "renderer.h"

namespace bss {
class GlyphRenderer;
}


/**
 * @brief The GlyphRenderer class This render glyps, they are small colored
 * lines.
 */
class GlyphRenderer : public Renderer
{
public:
    GlyphRenderer();
    ~GlyphRenderer();

    /**
     * @brief draw Draw all glyphs.
     */
    virtual void draw() override;
};

#endif // GLYPHRENDERER_H
