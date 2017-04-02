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

#include "stationsverticesbuilder.h"

#include "station.h"

StationsVerticesBuilder::StationsVerticesBuilder()
{

}

QVector<float> StationsVerticesBuilder::build(const QVector<Station>& stations) const
{
    // x and y;
    const unsigned short positionTupleSize = 2;
    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> stationsVertices;
    const int stationsVerticesCount = stations.size();
    stationsVertices.reserve(stationsVerticesCount * positionTupleSize * colorTupleSize);

    for (const Station s : stations)
    {
        const float maxLongitude = 180.f;
        const float maxLatitude = 90.f;

        stationsVertices.append(s.longitude / maxLongitude);
        stationsVertices.append(s.latitude / maxLatitude);

        stationsVertices.append(1.f);
        stationsVertices.append(1.f);
        stationsVertices.append(0.3f);
    }

    return stationsVertices;
}
