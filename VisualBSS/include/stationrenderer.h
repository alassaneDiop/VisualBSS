#ifndef STATIONRENDER_H
#define STATIONRENDER_H

#include "renderer.h"


namespace bss {
class StationRenderer;
}

/**
 * @brief The StationRenderer class Class that draw stations with OpenGL
 * functions
 */
class StationRenderer : public Renderer
{
public:
    StationRenderer();
    virtual ~StationRenderer();

    /**
     * @brief draw Fonction that draws stations with points.
     */
    virtual void draw() override;

    /**
     * @brief drawFilledCircle Fonction that draws stations with circles, unused
     * fonction.
     * @param triangleAmount Number of triangles that are used to draw circles,
     * more their is triangle, more the circle is smooth. Less their is, more
     * the circle will look like a polygon.
     * @param radius Radius of the circle.
     */
    void drawFilledCircle(unsigned int triangleAmount, unsigned int radius);

};

#endif // STATIONRENDER_H
