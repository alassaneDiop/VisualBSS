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
