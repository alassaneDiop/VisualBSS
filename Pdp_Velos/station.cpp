#include "station.h"

#include <QtMath>

#include "trip.h"


qreal Station::distance(const Station& to) const
{
    // Algorithm from https://openclassrooms.com/forum/sujet/calcul-d-une-distance-95555

    const qreal earthRadius = 6378000;

    const qreal fromLat = qDegreesToRadians(latitude);
    const qreal fromLon = qDegreesToRadians(longitude);
    const qreal toLat = qDegreesToRadians(to.latitude);
    const qreal toLon = qDegreesToRadians(to.longitude);

    const qreal a = qSin(fromLat) * qSin(toLat);
    const qreal b = qCos(fromLat) * qCos(toLat) * qCos(toLon - fromLon);

    return earthRadius * qAcos(a + b);
}

qreal Station::direction(const Station& to) const
{
    // https://en.wikipedia.org/wiki/Azimuth
    const float x = (to.longitude - longitude);
    const float y = (to.latitude - latitude);
    return qRadiansToDegrees(qAtan2(y, x)) + 180;
}

void Station::appendArrival(const Trip& trip)
{
    arrivalsIds.append(trip.id);
    updateAppend(trip);
}

void Station::appendDeparture(const Trip& trip)
{
    departuresIds.append(trip.id);
    updateAppend(trip);
}

void Station::appendCycle(const Trip& trip)
{
    cyclesIds.append(trip.id);
    updateAppend(trip);
}

void Station::updateAppend(const Trip& trip)
{
    flow++;

    const int totalTripsCount = arrivalsIds.size() + departuresIds.size() + cyclesIds.size();
    const int totalDuration = (flow * avgTripDuration) + trip.duration;
    avgTripDuration = totalDuration / totalTripsCount;
}
