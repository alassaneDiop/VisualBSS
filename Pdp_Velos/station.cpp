#include "station.h"


Station::Station() :
    Station(StationData())
{

}

Station::Station(const StationData& data) :
    m_id(data.id),
    m_name(data.name),
    m_latitude(data.latitude),
    m_longitude(data.longitude),
    m_avgTripDuration(QTime(data.avgTripDurationMsec)),
    m_maxTripsDistance(data.maxTripDistance),
    m_originDesinationFlow(data.originDesinationFlow),
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
