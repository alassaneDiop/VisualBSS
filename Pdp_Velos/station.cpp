#include "station.h"
#include "trip.h"

Station::Station() :
    m_isValid(false),
    m_isNull(true),
    m_latitude(0),
    m_longitude(0),
    m_minTripsDistance(0),
    m_avgTripsDistance(0),
    m_maxTripsDistance(0),
    m_originDesinationFlow(0)
{

}

Station::Station(const QString& name, const qreal latitude, const qreal longitude) :
    m_name(name), m_latitude(latitude), m_longitude(longitude)
{
    if (m_name.isEmpty())
    {
        m_isNull = true;
        m_isValid = false;
    }
    else
    {
        m_isNull = false;
        m_isValid = true;
        m_minTripsDuration = calculateMinTripsDuration();
        m_avgTripsDuration = calculateAvgTripsDuration(),
        m_maxTripsDuration = calculateMaxTripsDuration();
        m_minTripsDistance = calculateMinTripsDistance();
        m_avgTripsDistance = calculateAvgTripsDistance();
        m_maxTripsDistance = calculateMaxTripsDistance();
        m_originDesinationFlow = calculateOriginDesinationFlow();
    }
}

QSet<const Trip*>::iterator Station::insertIncomingTrip(const Trip* trip)
{
    if (!(trip && trip->isValid()))
        return QSet<const Trip*>::iterator();
    else
    {
        const QSet<const Trip*>::iterator end = m_incomingTrips.end();
        if (end ==  m_incomingTrips.insert(trip))
            onTripInserted(trip);

        return end;
    }
}

QSet<const Trip*>::iterator Station::insertOutgoingTrip(const Trip* trip)
{
    if (!(trip && trip->isValid()))
        return QSet<const Trip*>::iterator();
    else
    {
        const QSet<const Trip*>::iterator end = m_outgoingTrips.end();
        if (end ==  m_outgoingTrips.insert(trip))
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
        if (m_minTripsDuration > trip->getDuration())
            m_minTripsDuration = trip->getDuration();

        if (m_maxTripsDuration > trip->getDuration())
            m_maxTripsDuration = trip->getDuration();

        m_minTripsDistance = qMin(getMinTripsDistance(), trip->getDistance());
        m_maxTripsDistance = qMax(getMaxTripsDistance(), trip->getDistance());

        // a verifier
        const int tripsCount = getIncomingTrips().size() + getOutgoingTrips().size();
        const int ms = ((tripsCount - 1) * getAvgTripsDuration().msec() + trip->getDuration().msec()) / tripsCount;
        m_avgTripsDuration = QTime(0, 0, 0).addMSecs(ms);
        m_avgTripsDistance = ((tripsCount - 1) * getAvgTripsDistance() + trip->getDistance()) / tripsCount;
        m_originDesinationFlow++;
    }
}

QTime Station::calculateMinTripsDuration() const
{
    // TODO : calculateMinTripsDuration
    return QTime();
}

QTime Station::calculateAvgTripsDuration() const
{
    // TODO : calculateAvgTripsDuration
    return QTime();
}

QTime Station::calculateMaxTripsDuration() const
{
    // TODO : calculateMaxTripsDuration
    return QTime();
}

qreal Station::calculateMinTripsDistance() const
{
    // TODO : calculateMinTripsDistance
    return 0;
}

qreal Station::calculateAvgTripsDistance() const
{
    // TODO : calculateAvgTripsDistance
    return 0;
}

qreal Station::calculateMaxTripsDistance() const
{
    // TODO : calculateMaxTripsDistance
    return 0;
}

int Station::calculateOriginDesinationFlow() const
{
    // TODO : calculateOriginDesinationFlow
    return 0;
}

