#ifndef GLYPHRENDERER_H
#define GLYPHRENDERER_H

#include "renderer.h"

class GlyphRenderer : public Renderer
{
public:
    GlyphRenderer();
    ~GlyphRenderer();

    virtual void draw() override;
};

#endif // GLYPHRENDERER_H
