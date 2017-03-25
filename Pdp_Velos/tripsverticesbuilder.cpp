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

    // TODO: damien : renommer des variables
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

    // FIXME: couleur
    drawTrips(arrivals, tripsVertices, tripsVerticesCount, bss::ARRIVAL_ORIGIN_COLOR, bss::ARRIVAL_DESTINATION_COLOR);
    drawTrips(departures, tripsVertices, tripsVerticesCount, bss::DEPARTURE_ORIGIN_COLOR, bss::DEPARTURE_DESTINATION_COLOR);

    return tripsVertices;
}





