#ifndef GLYPHSVERTICESBUILDER_H
#define GLYPHSVERTICESBUILDER_H

#include <QVector>
#include <QPointF>

namespace bss {
class GlyphsVerticesBuilder;
}


struct Trip;
class Data;
class GlyphsVerticesBuilder
{
public:
    GlyphsVerticesBuilder();

    QVector<float> build(const int& timelineMatrixHeight,
                         const int& timelineMatrixWidth,
                         const QVector<QVector<int>>& arrivalsIdsList,
                         const QVector<QVector<int>>& departuresIdsList,
                         const QVector<QVector<int>>& cyclesIdsList,
                         const Data& data,
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
                             const QVector<int>& tripsIds,
                             const Data& data,
                             float posX, float posY,
                             const QVector<float>& color) const;
};

#endif // GLYPHSVERTICESBUILDER_H
