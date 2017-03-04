#include "trip.h"
#include "station.h"

Trip::Trip() :
    m_isValid(false),
    m_isNull(true),
    m_isCyclic(false),
    m_startStation(nullptr),
    m_endStation(nullptr),
    m_direction(0),
    m_distance(0)
{

}

Trip::Trip(Station* const& startStation,
           Station* const& endStation,
           const QDateTime& startDateTime,
           const QDateTime& endDateTime)
{
    m_isValid = startDateTime.isValid() && endDateTime.isValid()
            && (startDateTime <= endDateTime)
            && startStation && startStation->isValid()
            && endStation && endStation->isValid();

    if (!isValid())
    {
        m_isNull = true;
        m_isCyclic = false;
        m_distance = 0;
        m_direction = 0;
    }
    else
    {
        m_isNull = false;
        m_isCyclic = (startStation == endStation);
        m_direction = startStation->direction(endStation);
        m_startStation = startStation;
        m_endStation = endStation;
        m_startDateTime = startDateTime;
        m_endDateTime = endDateTime;

        const int msecTo = startDateTime.msecsTo(endDateTime);
        m_duration = QTime(0, 0, 0).addMSecs(msecTo);

        if (isCyclic())
        {
            // TODO : "For cyclic trips, we estimated the distance by multiplying the duration by the average speed of 2.7m/s."
            m_distance = 0;
            m_startStation->appendCycle(this);
        }
        else
        {
            m_distance = startStation->distance(endStation);
            m_startStation->appendDeparture(this);
            m_endStation->appendArrival(this);
        }
    }
}

QString Trip::toString() const
{
    QString result;
    result += "start station :\t" + (getStartStation() ? getStartStation()->toString() : "null") + '\n';
    result += "end station :\t" + (getEndStation() ? getEndStation()->toString() : "null") + '\n';
    result += "start time :\t" + getStartDateTime().toString() + '\n';
    result += "end time :\t" + getEndDateTime().toString() + '\n';
    result += "duration :\t" + getDuration().toString();
    return result;
}
