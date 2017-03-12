#include "station.h"
#include "trip.h"
#include <QtMath>

qreal Station::distance(const Station& to) const
{
    // https://openclassrooms.com/forum/sujet/calcul-d-une-distance-95555

    // Earth's radius approximation
    const qreal R = 6378000;

    // converts degrees to radians
    const qreal fromLat = qDegreesToRadians(latitude);
    const qreal fromLon = qDegreesToRadians(longitude);
    const qreal toLat = qDegreesToRadians(to.latitude);
    const qreal toLon = qDegreesToRadians(to.longitude);

    const qreal a = qSin(fromLat) * qSin(toLat);
    const qreal b = qCos(fromLat) * qCos(toLat) * qCos(toLon - fromLon);
    return R * qAcos(a + b);
}

qreal Station::direction(const Station& to) const
{
    // https://en.wikipedia.org/wiki/Azimuth
    const float x = (to.longitude - longitude);
    const float y = (to.latitude - latitude);
    return qRadiansToDegrees(qAtan2(y, x));
}

void Station::appendArrival(const Trip& trip)
{
    arrivalsId.append(trip.id);
    updateAppend(trip);
}

void Station::appendDeparture(const Trip& trip)
{
    departuresId.append(trip.id);
    updateAppend(trip);
}

void Station::appendCycle(const Trip& trip)
{
    cyclesId.append(trip.id);
    updateAppend(trip);
}

void Station::updateAppend(const Trip& trip)
{
    // calculates the maximum trip distance
    maxTripDistance = qMax(maxTripDistance, trip.distance);

    // TODO : originDesinationFlow : a vérifier
    originDestinationFlow++;

    // calculates the average trips duration
    const int totalTripsCount = arrivalsId.size() + departuresId.size() + cyclesId.size();
    const int totalDuration = (originDestinationFlow * avgTripDuration) + trip.duration;
    avgTripDuration = totalDuration / totalTripsCount;
}
