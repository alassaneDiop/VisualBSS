#include "trip.h"

Trip::Trip(const Station& startStation, const Station& endStation,
           const QDateTime& startDateTime, const QDateTime& endDateTime,
           const QTime& duration) :
    m_startStation(startStation),
    m_endStation(endStation),
    m_startDateTime(startDateTime),
    m_endDateTime(endDateTime),
    m_duration(duration),
    m_distance(calculateDistance(startStation, endStation)),
    m_direction(calculateDirection(startStation, endStation))
{
    m_isValid = isValid(*this);
    m_isNull = isNull(*this);
}

Trip::operator QString() const
{
    QString result;
    result += "start station :\t" + getStartStation() + '\n';
    result += "end station :\t" + getEndStation() + '\n';
    result += "start time :\t" + getStartDateTime().toString() + '\n';
    result += "end time :\t" + getEndDateTime().toString() + '\n';
    result += "duration :\t" + getDuration().toString();
    return result;
}

bool Trip::operator ==(const Trip& trip) const
{
    return getStartStation() == trip.getStartStation()
            && getEndStation() == trip.getEndStation()
            && getStartDateTime() == trip.getStartDateTime()
            && getEndDateTime() == trip.getEndDateTime()
            && getDuration() == trip.getDuration()
            && qFuzzyCompare(getDistance() + 1, trip.getDistance() + 1)
            && qFuzzyCompare(getDirection() + 1, trip.getDirection() + 1);
}

bool Trip::isValid(const Trip& t)
{
    return t.getStartStation().isValid() && t.getEndStation().isValid()
            && t.getStartDateTime().isValid() && t.getEndDateTime().isValid()
            && t.getDuration().isValid() && (t.getDistance() > 0);
}

bool Trip::isNull(const Trip& t)
{
    return !isValid(t);
}

qreal Trip::calculateDistance(const Station& s1, const Station& s2)
{
    return 1;
}

qreal Trip::calculateDirection(const Station& s1, const Station& s2)
{
    return 0;
}
