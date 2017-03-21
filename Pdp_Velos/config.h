#ifndef CONFIG_H
#define CONFIG_H

template<typename T>
class QVector;

namespace bss {

static const unsigned int NB_OF_HOURS = 24;

static const int TIMELINE_OFFSET_X = 10;

// Heigth in pixel
static const unsigned int GLYPH_HEIGHT = 10;
static const unsigned int SPACE_BETWEEN_GLYPHS = 15;


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
