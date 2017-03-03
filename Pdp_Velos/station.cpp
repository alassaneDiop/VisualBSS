#include "station.h"
#include "trip.h"

Station::Station() :
    m_isValid(false),
    m_isNull(true),
    m_latitude(0),
    m_longitude(0)
{

}

Station::Station(const QString& name, const qreal latitude, const qreal longitude) :
    m_name(name), m_latitude(latitude), m_longitude(longitude)
{
    if (name.isEmpty())
    {
        m_isNull = true;
        m_isValid = false;
    }
    else
    {
        m_isNull = false;
        m_isValid = true;
    }
}

const QSet<const Trip*>::iterator Station::insertArrival(const Trip* trip)
{
    if (!(trip && trip->isValid()))
        return QSet<const Trip*>::iterator();
    else
    {
        const QSet<const Trip*>::iterator end = m_arrivals.end();
        if (end ==  m_arrivals.insert(trip))
            onTripInserted(trip);

        return end;
    }
}

const QSet<const Trip*>::iterator Station::insertDeparture(const Trip* trip)
{
    if (!(trip && trip->isValid()))
        return QSet<const Trip*>::iterator();
    else
    {
        const QSet<const Trip*>::iterator end = m_departures.end();
        if (end ==  m_departures.insert(trip))
            onTripInserted(trip);

        return end;
    }
}

const QSet<const Trip*>::iterator Station::insertCycle(const Trip* trip)
{
    if (!(trip && trip->isValid() && trip->isCyclic()))
        return QSet<const Trip*>::iterator();
    else
    {
        const QSet<const Trip*>::iterator end = m_cycles.end();
        if (end ==  m_cycles.insert(trip))
            onTripInserted(trip);

        return end;
    }
}

qreal Station::distance(const Station* to) const
{
    // TODO : distance
    return 0;
}

qreal Station::direction(const Station* to) const
{
    // TODO : direction
    return 0;
}

bool Station::operator==(const Station& other) const
{
    return (getName() == other.getName());
}

QString Station::toString() const
{
    QString result;
    result += "name : " + getName() + '\t';
    result += "latitude : " + QString:: number(getLatitude()) + '\t';
    result += "longitude : " + QString:: number(getLongitude());
    return result;
}

void Station::onTripInserted(const Trip* trip)
{
    // TODO : onTripInserted
    if (trip)
    {
        m_maxTripsDistance = qMax(getMaxTripsDistance(), trip->getDistance());

        // a verifier
        const int tripsCount = getArrivalsCount() + getDeparturesCount() + getCyclesCount();
        const int ms = ((tripsCount - 1) * getAvgTripsDuration().msec() + trip->getDuration().msec()) / tripsCount;
        m_avgTripsDuration = QTime(0, 0, 0).addMSecs(ms);
        m_avgTripsDistance = ((tripsCount - 1) * getAvgTripsDistance() + trip->getDistance()) / tripsCount;
        m_originDesinationFlow++;
    }
}
