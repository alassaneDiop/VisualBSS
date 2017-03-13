#ifndef GLYPHRENDERER_H
#define GLYPHRENDERER_H

#include "renderer.h"

namespace bss {
class GlyphRenderer;
}

class GlyphRenderer : public Renderer
{
public:
    GlyphRenderer();
    ~GlyphRenderer();

    virtual void draw() override;
};

#endif // GLYPHRENDERER_H
