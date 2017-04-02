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

#include "tripsverticesbuilder.h"

#include "trip.h"
#include "station.h"
#include "config.h"

TripsVerticesBuilder::TripsVerticesBuilder()
{

}




QVector<float> TripsVerticesBuilder::build(const QVector<Station>& stations,
                                           const QVector<Trip>& arrivals,
                                           const QVector<Trip>& departures,
                                           const QVector<Trip>& cycles) const
{
    // x and y;
    const unsigned short positionTupleSize = 2;
    // 1 trip has 2 points (start/end)
    const unsigned short pointPerTrip = 2;
    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> tripsVertices;
    const int arrivalsVertices = arrivals.size() * pointPerTrip * positionTupleSize * colorTupleSize;
    const int departuresVertices = departures.size() * pointPerTrip * positionTupleSize * colorTupleSize;
    const int cyclesVertices = cycles.size() * positionTupleSize * colorTupleSize;

    tripsVertices.reserve(arrivalsVertices + departuresVertices + cyclesVertices);

    int tripsVerticesCount = 0;

    const auto drawTrips = [this, &stations](  const QVector<Trip>& trips,
            QVector<float>& tripsVertices,
            int &verticesCount,
            const QVector<float>& originColor,
            const QVector<float>& destinationColor)
    {
        for (const Trip t : trips)
        {
            const int startStationId = t.startStationId;
            const Station startStation = stations.at(startStationId);

            const float maxLongitude = 180.f;
            const float maxLatitude = 90.f;

            tripsVertices.append((float) (startStation.longitude / maxLongitude));
            tripsVertices.append((float) (startStation.latitude / maxLatitude));
            tripsVertices.append(originColor);


            const int endStationId = t.endStationId;
            const Station endStation = stations.at(endStationId);

            tripsVertices.append((float) (endStation.longitude / maxLongitude));
            tripsVertices.append((float) (endStation.latitude / maxLatitude));
            tripsVertices.append(destinationColor);

            verticesCount += 2;
        }
    };

    drawTrips(arrivals, tripsVertices, tripsVerticesCount, bss::ARRIVAL_ORIGIN_COLOR, bss::ARRIVAL_DESTINATION_COLOR);
    drawTrips(departures, tripsVertices, tripsVerticesCount, bss::DEPARTURE_ORIGIN_COLOR, bss::DEPARTURE_DESTINATION_COLOR);

    return tripsVertices;
}





