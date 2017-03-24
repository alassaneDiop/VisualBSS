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
 * According to the article by Guilherme Oliveira : "Visual analysis of a bike sharing system"
 * "For cyclic trips, we estimated the distance by multiplying the duration by
 * the average speed of 2.7m/s."
 */
static const qreal AVG_SPEED = 2.7;

// Informations for drawing glyphs on timeline matrix
static const int NB_OF_HOURS = 24;
static const int TIMELINE_OFFSET_X = 10;

// Heigth in pixel
static const int GLYPH_HEIGHT = 10;
static const int SPACE_BETWEEN_GLYPHS = 15;

static const int MATRIX_SCROLL_COEFF = 2;


// TRIPS COLORS
static const QVector<float> ARRIVAL_ORIGIN_COLOR = QVector<float>({ 1.f, 0.f, 0.f });
static const QVector<float> ARRIVAL_DESTINATION_COLOR = QVector<float>({ 1.f, 1.f, 0.f });

static const QVector<float> DEPARTURE_ORIGIN_COLOR = QVector<float>({ 0.f, 1.f, 1.f  });
static const QVector<float> DEPARTURE_DESTINATION_COLOR = QVector<float>({ 0.f, 0.f, 1.f });

// GLYPHS COLORS
static const QVector<float> GLYPH_ARRIVAL_COLOR = QVector<float>({ 0.f, 0.f, 1.f });
static const QVector<float> GLYPH_DEPARTURE_COLOR = QVector<float>({ 1, 0.f, 0.f });

}

#endif // CONFIG_H
