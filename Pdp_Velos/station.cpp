#include "station.h"
#include "trip.h"


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
    // calcul maxTripDistance
    maxTripDistance = qMax(maxTripDistance, trip.distance);

    // TODO : avgTripDuration : a vérifier
    const int totalTripsCount = arrivalsId.size() + departuresId.size() + cyclesId.size();
    const int totalDuration = (originDestinationFlow * avgTripDuration) + trip.duration;
    avgTripDuration = totalDuration / totalTripsCount;

    // TODO : originDesinationFlow : a vérifier
    originDestinationFlow++;
}

qreal Station::distance(const Station& to) const
{
    // TODO : distance
    return 0;
}

qreal Station::direction(const Station& to) const
{
    // TODO : direction
    return 0;
}

bool Station::operator==(const Station& other) const
{
    return (id == other.id);
}

QString Station::toString() const
{
    QString result;
    result += "name : " + name + '\t';
    result += "latitude : " + QString:: number(latitude) + '\t';
    result += "longitude : " + QString::number(longitude);
    return result;
}
