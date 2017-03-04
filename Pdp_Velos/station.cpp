#include "station.h"


Station::Station() :
    m_id(-1),
    m_latitude(0),
    m_longitude(0),
    m_avgTripsDistance(0),
    m_maxTripsDistance(0),
    m_originDesinationFlow(0)
{

}

Station::Station(const StationData& data)
{
    m_id = data.id;
    m_name = data.name;
    m_latitude = data.latitude;
    m_longitude = data.longitude;
    m_avgTripsDuration = data.avgTripsDuration;
    m_maxTripsDistance = data.maxTripsDistance;
    m_originDesinationFlow = data.originDesinationFlow;
    m_arrivalsId = data.arrivalsId;
    m_departuresId = data.departuresId;
    m_cyclesId = data.cyclesId;
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
    return (getId() == other.getId());
}

QString Station::toString() const
{
    QString result;
    result += "name : " + getName() + '\t';
    result += "latitude : " + QString:: number(getLatitude()) + '\t';
    result += "longitude : " + QString:: number(getLongitude());
    return result;
}
