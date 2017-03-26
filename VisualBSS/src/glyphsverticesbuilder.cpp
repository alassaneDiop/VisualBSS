#include "glyphsverticesbuilder.h"

#include <QtMath>

#include "trip.h"
#include "config.h"


GlyphsVerticesBuilder::GlyphsVerticesBuilder()
{

}

QVector<float> GlyphsVerticesBuilder::build(const int& timelineMatrixHeight,
                                            const int& timelineMatrixWidth,
                                            const QVector<QVector<Trip>>& arrivals,
                                            const QVector<QVector<Trip>>& departures,
                                            const QVector<QVector<Trip>>& cycles,
                                            const bool& showDistance) const
{
    Q_UNUSED(cycles);
    Q_UNUSED(showDistance);

    const float glyphIntervalX = timelineMatrixWidth / (bss::NB_OF_HOURS * 1.f);
    const float glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

    QVector<float> glyphsVertices;
    // TODO: Damien : calculer
    //glyphsData.reserve();

    int i = 0;
    int stationIndex = 0;
    for (const QVector<Trip> t : arrivals)
    {
        const float posX = (bss::TIMELINE_OFFSET_X + (i * glyphIntervalX));
        const float posY = stationIndex * glyphIntervalY;

        glyphsVertices.append(buildGlyphsVertices(timelineMatrixHeight, timelineMatrixWidth, t, posX, posY, bss::GLYPH_ARRIVAL_COLOR));

        i++;
        i = i % 24;

        // if i equals 0, thats means we move to another station (and a new line to draw glyphs)
        if (i == 0)
            stationIndex++;
    }

    i = 0;
    stationIndex = 0;
    for (const QVector<Trip> t : departures)
    {
        const float posX = (bss::TIMELINE_OFFSET_X + (i * glyphIntervalX));
        const float posY = stationIndex * glyphIntervalY;

        glyphsVertices.append(buildGlyphsVertices(timelineMatrixHeight, timelineMatrixWidth, t, posX, posY, bss::GLYPH_DEPARTURE_COLOR));

        i++;
        i = i % 24;

        // if i equals 0, thats means we move to another station (and a new line to draw glyphs)
        if (i == 0)
            stationIndex++;
    }

    return glyphsVertices;
}

QPointF GlyphsVerticesBuilder::rotateAround(const float& angle, const QPointF& position, const QPointF& target)
{
    const float sin = qSin(angle);
    const float cos = qCos(angle);

    // Translate positionX/Y to the origin
    float pX = target.x() - position.x();
    float pY = target.y() - position.y();

    // Do the rotation
    float newX = pX * cos - pY * sin;
    float newY = pX * sin + pY * cos;

    // Translate back
    newX += position.x();
    newY += position.y();

    return QPointF(newX, newY);
}

QVector<float> GlyphsVerticesBuilder::buildTripVertices(const int& timelineMatrixHeight,
                                               const int& timelineMatrixWidth,
                                               const Trip& t,
                                               float posX, float posY,
                                               const QVector<float>& color) const
{
    const int colorTupleSize = 3;
    const int positionTupleSize = 2;
    const int numberOfPointPerTrip = 2;

    QVector<float> tripData;
    tripData.reserve((colorTupleSize + positionTupleSize) * numberOfPointPerTrip);

    // VERTEX #1
    const float positionXP1 = posX;
    const float positionYP1 = posY - (bss::GLYPH_HEIGHT / 2.f);

    const float rotationCenterX = posX;
    const float rotationCenterY = posY;

    const float tripDirection = t.direction;

    QPointF positionAfterRotation;
    positionAfterRotation = rotateAround(tripDirection,
                                   QPointF(positionXP1, positionYP1),
                                   QPointF(rotationCenterX, rotationCenterY));

    const float newPosXP1 = positionAfterRotation.x() / timelineMatrixWidth * 2 - 1;
    const float newPosYP1 = positionAfterRotation.y() / timelineMatrixHeight * 2 - 1;

    tripData.append(newPosXP1);
    tripData.append(-newPosYP1);
    tripData.append(color);


    // VERTEX #2
    const float positionXP2 = posX;
    const float positionYP2 = posY + (bss::GLYPH_HEIGHT / 2.f);

    positionAfterRotation = rotateAround(tripDirection,
                                   QPointF(positionXP2, positionYP2),
                                   QPointF(rotationCenterX, rotationCenterY));

    const float newPosXP2 = positionAfterRotation.x() / timelineMatrixWidth * 2 - 1;
    const float newPosYP2 = positionAfterRotation.y() / timelineMatrixHeight * 2 - 1;

    tripData.append(newPosXP2);
    tripData.append(-newPosYP2);
    tripData.append(color);

    return tripData;
}

QVector<float> GlyphsVerticesBuilder::buildGlyphsVertices(const int& timelineMatrixHeight,
                                                          const int& timelineMatrixWidth,
                                                          const QVector<Trip>& trips,
                                                          float posX, float posY,
                                                          const QVector<float>& color) const
{
    QVector<float> glyph;
    for (const Trip t : trips)
        glyph.append(buildTripVertices(timelineMatrixHeight, timelineMatrixWidth, t, posX, posY, color));

    return glyph;
}


