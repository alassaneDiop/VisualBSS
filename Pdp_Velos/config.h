#ifndef CONFIG_H
#define CONFIG_H

template<typename T>
class QVector;

namespace bss {

static const int RANK_SIZE = 10;

static const bool PARALLEL_PARSING = true;

static const qreal AVG_SPEED = 2.7;

static const int NB_OF_HOURS = 24;

static const int TIMELINE_OFFSET_X = 10;

// Heigth in pixel
static const int GLYPH_HEIGHT = 10;
static const int SPACE_BETWEEN_GLYPHS = 15;


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
