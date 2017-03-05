#include "station.h"
#include "trip.h"

StationData::StationData(const int& id,
                         const QString& name,
                         const qreal& latitude,
                         const qreal& longitude) :
    id(id), name(name), latitude(latitude), longitude(longitude)
{

}

void StationData::appendArrival(const TripData& trip)
{
    arrivalsId.append(trip.id);
    updateAppend(trip);
}

void StationData::appendDeparture(const TripData& trip)
{
    departuresId.append(trip.id);
    updateAppend(trip);
}

void StationData::appendCycle(const TripData& trip)
{
    cyclesId.append(trip.id);
    updateAppend(trip);
}

void StationData::updateAppend(const TripData& trip)
{
    // calcul maxTripDistance
    maxTripDistance = qMax(maxTripDistance, trip.distance);

    // TODO : avgTripDurationMsec : a vérifier
    const int totalTripCount = arrivalsId.size() + departuresId.size() + cyclesId.size();
    const int totalDurationMsec = (odFlow * avgTripDurationMsec) + trip.durationMsec;
    avgTripDurationMsec = (totalDurationMsec / totalTripCount);

    // TODO : originDesinationFlow : a vérifier
    odFlow++;
}

Station::Station(const StationData& data) :
    m_id(data.id),
    m_name(data.name),
    m_latitude(data.latitude),
    m_longitude(data.longitude),
    m_avgTripDuration(QTime().addMSecs(data.avgTripDurationMsec)),
    m_maxTripsDistance(data.maxTripDistance),
    m_originDesinationFlow(data.odFlow),
    m_arrivalsId(data.arrivalsId),
    m_departuresId(data.departuresId),
    m_cyclesId(data.cyclesId)
{

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
    return (id() == other.id());
}

QString Station::toString() const
{
    QString result;
    result += "name : " + name() + '\t';
    result += "latitude : " + QString:: number(latitude()) + '\t';
    result += "longitude : " + QString:: number(longitude());
    return result;
}
