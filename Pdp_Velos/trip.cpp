#include "trip.h"

Trip::Trip() :
    m_id(-1),
    m_startStationId(-1),
    m_endStationId(-1),
    m_isCyclic(false),
    m_direction(0),
    m_distance(0)
{

}

Trip::Trip(const TripData& data)
{
    m_id = data.id;
    m_startStationId = data.startId;
    m_endStationId = data.endId;
    m_isCyclic = (data.startId == data.endId);
    m_direction = data.direction;
    m_distance = data.distance;
    m_startDateTime = data.startDateTime;
    m_endDateTime = data.endDateTime;

    const int msecTo = getStartDateTime().msecsTo(getEndDateTime());
    m_duration = QTime(0, 0, 0).addMSecs(msecTo);

    if (isCyclic())
    {
        // TODO : "For cyclic trips, we estimated the distance by multiplying the duration by the average speed of 2.7m/s."
        m_distance = 0;
    }
    else
    {
        //m_distance = this->distance(endStation);
    }
}

QString Trip::toString() const
{
    QString result;
    result += "start station id :\t" + QString::number(getStartStationId()) + '\n';
    result += "end station id :\t" + QString::number(getEndStationId()) + '\n';
    result += "start time :\t" + getStartDateTime().toString() + '\n';
    result += "end time :\t" + getEndDateTime().toString() + '\n';
    result += "duration :\t" + getDuration().toString();
    return result;
}
