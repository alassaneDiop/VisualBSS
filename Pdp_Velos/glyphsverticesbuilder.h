#ifndef GLYPHSVERTICESBUILDER_H
#define GLYPHSVERTICESBUILDER_H

#include <QVector>
#include <QPointF>

namespace bss {
class GlyphsVerticesBuilder;
}


struct Trip;
class GlyphsVerticesBuilder
{
public:
    GlyphsVerticesBuilder();

    QVector<float> build(const int& timelineMatrixHeight,
                         const int& timelineMatrixWidth,
                         const QVector<QVector<Trip>>& arrivals,
                         const QVector<QVector<Trip>>& departures,
                         const QVector<QVector<Trip>>& cycles,
                         const bool& showDistance) const;

private:
    static QPointF rotateAround(const float& angle, const QPointF& position, const QPointF& target);

    QVector<float> buildTripVertices(const int& timelineMatrixHeight,
                            const int& timelineMatrixWidth,
                            const Trip& t,
                            float posX, float posY,
                            const QVector<float>& color) const;

    QVector<float> buildGlyphsVertices(const int& timelineMatrixHeight,
                             const int& timelineMatrixWidth,
                             const QVector<Trip>& trips,
                             float posX, float posY,
                             const QVector<float>& color) const;
};

#endif // GLYPHSVERTICESBUILDER_H
