/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include "glyphsverticesbuilder.h"

#include <QtMath>

#include "trip.h"
#include "config.h"
#include "data.h"

GlyphsVerticesBuilder::GlyphsVerticesBuilder()
{

}

QVector<float> GlyphsVerticesBuilder::build(const int& timelineMatrixHeight,
                     const int& timelineMatrixWidth,
                     const QVector<QVector<int>>& arrivalsIdsList,
                     const QVector<QVector<int>>& departuresIdsList,
                     const QVector<QVector<int>>& cyclesIdsList,
                     const Data& data,
                     const bool& showDistance) const
{
    Q_UNUSED(cyclesIdsList);
    Q_UNUSED(showDistance);

    const float glyphIntervalX = timelineMatrixWidth / (bss::NB_OF_HOURS * 1.f);
    const float glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

    QVector<float> glyphsVertices;
    const int pointPerTrips = 2;
    const int positionTupleSize = 2;
    const int colorTupleSize = 3;

    const int arrivalsVerticesCount = arrivalsIdsList.size() * pointPerTrips * positionTupleSize * colorTupleSize;
    const int departuresVerticesCount = departuresIdsList.size() * pointPerTrips * positionTupleSize * colorTupleSize;
    const int cyclesVerticesCount = cyclesIdsList.size() * pointPerTrips * positionTupleSize * colorTupleSize;
    glyphsVertices.reserve(arrivalsVerticesCount + departuresVerticesCount + cyclesVerticesCount);

    int i = 0;
    int stationIndex = 0;
    for (const QVector<int> arrivalsIds : arrivalsIdsList)
    {
        const float posX = (bss::TIMELINE_OFFSET_X + (i * glyphIntervalX));
        const float posY = stationIndex * glyphIntervalY;

        glyphsVertices.append(buildGlyphsVertices(timelineMatrixHeight,
                                                  timelineMatrixWidth,
                                                  arrivalsIds, data, posX, posY,
                                                  bss::GLYPH_ARRIVAL_COLOR));

        i++;
        i = i % 24;

        // if i equals 0, thats means we move to another station (and a new line to draw glyphs)
        if (i == 0)
            stationIndex++;
    }

    i = 0;
    stationIndex = 0;
    for (const QVector<int> departuresIds : departuresIdsList)
    {
        const float posX = (bss::TIMELINE_OFFSET_X + (i * glyphIntervalX));
        const float posY = stationIndex * glyphIntervalY;

        glyphsVertices.append(buildGlyphsVertices(timelineMatrixHeight,
                                                  timelineMatrixWidth,
                                                  departuresIds, data, posX, posY,
                                                  bss::GLYPH_DEPARTURE_COLOR));

        i++;
        i = i % 24;

        // if i equals 0, thats means we move to another station (and a new line to draw glyphs)
        if (i == 0)
            stationIndex++;
    }

    return glyphsVertices;
}

QPointF GlyphsVerticesBuilder::rotateAround(const float& angle,
                                            const QPointF& position,
                                            const QPointF& target)
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
                                                          const QVector<int>& tripsIds,
                                                          const Data& data,
                                                          float posX, float posY,
                                                          const QVector<float>& color) const
{
    QVector<float> glyph;
    for (const int tripId : tripsIds)
    {
        const Trip t = data.trip(tripId);
        glyph.append(buildTripVertices(timelineMatrixHeight, timelineMatrixWidth, t, posX, posY, color));
    }

    return glyph;
}


