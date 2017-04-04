#ifndef CONFIG_H
#define CONFIG_H

template<typename T>
class QVector;

namespace bss {

/**
 * Indicates the number of stations to rank when they are sorted by a property.
 * Top stations and last stations will be displayed in list views once sorted.
 */
static const int RANK_SIZE = 10;

/**
 * Indicates wheter or not the data files parsing should be parallelized.
 */
static const bool PARALLEL_PARSING = true;

/**
 * According to the article by "Visual analysis of bike-sharing systems Guilherme" by
 * N. Oliveira, Jose L. Sotomayor, Rafael P. Torchelsen, Cláudio T. Silva, João L.D. Comba :
 * "For cyclic trips, we estimated the distance by multiplying the duration by
 * the average speed of 2.7m/s."
 */
static const qreal AVG_SPEED = 2.7;

/**
 * @brief NB_OF_HOURS Display trips from time 0 to time NB_OF_HOURS value in
 * the timeline matrix.
 */
static const int NB_OF_HOURS = 24;

/**
 * @brief TIMELINE_OFFSET_X Set an offset of 10 pixels on the timeline matrix view
 * If its set to 0, glyphs an hours will not be shown correctly.
 */
static const int TIMELINE_OFFSET_X = 10;

/**
 * @brief GLYPH_HEIGHT Each glyphs in the timeline matrix will be displayed with
 * an heigth of 10 pixels.
 */
static const int GLYPH_HEIGHT = 10;

/**
 * @brief SPACE_BETWEEN_GLYPHS Indicates the space beetwen 2 glyphs in pixels.
 */
static const int SPACE_BETWEEN_GLYPHS = 15;

/**
 * @brief MATRIX_SCROLL_COEFF Indicates the sensibility of the scroll value for
 * the vertical scroll bar in the timeline matrix.
 */
static const int MATRIX_SCROLL_COEFF = 2;


// TRIPS COLORS
/**
 * @brief ARRIVAL_ORIGIN_COLOR All origin's arrival trips will be displayed by a
 * red line with a gradient to ARRIVAL_DESTINATION_COLOR value.
 */
static const QVector<float> ARRIVAL_ORIGIN_COLOR = QVector<float>({ 1.f, 0.f, 0.f });

/**
 * @brief ARRIVAL_DESTINATION_COLOR All destination's arrival trips will be displayed by a
 * yellow line with a gradient to ARRIVAL_ORIGIN_COLOR value.
 */
static const QVector<float> ARRIVAL_DESTINATION_COLOR = QVector<float>({ 1.f, 1.f, 0.f });

/**
 * @brief DEPARTURE_ORIGIN_COLOR All origin's departure trips will be displayed by a
 * cyan line with a gradient to DEPARTURE_DESTINATION_COLOR value.
 */
static const QVector<float> DEPARTURE_ORIGIN_COLOR = QVector<float>({ 0.f, 1.f, 1.f  });

/**
 * @brief DEPARTURE_DESTINATION_COLOR All origin's departure trips will be displayed by a
 * blue line with a gradient to DEPARTURE_ORIGIN_COLOR value.
 */
static const QVector<float> DEPARTURE_DESTINATION_COLOR = QVector<float>({ 0.f, 0.f, 1.f });


// GLYPHS COLORS
/**
 * @brief GLYPH_ARRIVAL_COLOR Each glyphs in the timeline matrix will be dysplayed
 * by a blue line if trips are arrival of the converned station.
 */
static const QVector<float> GLYPH_ARRIVAL_COLOR = QVector<float>({ 1.f, 0.f, 0.f });

/**
 * @brief GLYPH_DEPARTURE_COLOR Each glyphs in the timeline matrix will be dysplayed
 * by a red line if trips are arrival of the converned station.
 */
static const QVector<float> GLYPH_DEPARTURE_COLOR = QVector<float>({ 0.f, 0.f, 1.f });

}

#endif // CONFIG_H
