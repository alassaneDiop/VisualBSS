#include "trip.h"

Trip::Trip() :
    Trip(TripData())
{

}

Trip::Trip(const TripData& data) :
    m_id(data.id),
    m_startStationId(data.startId),
    m_endStationId(data.endId),
    m_isCyclic(data.isCyclic),
    m_direction(data.direction),
    m_distance(data.distance),
    m_startDateTime(data.startDateTime),
    m_endDateTime(data.endDateTime),
    m_duration(QTime(data.durationMsec))
{
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
    result += "start station id :\t" + QString::number(startStationId()) + '\n';
    result += "end station id :\t" + QString::number(endStationId()) + '\n';
    result += "start time :\t" + startDateTime().toString() + '\n';
    result += "end time :\t" + endDateTime().toString() + '\n';
    result += "duration :\t" + duration().toString();
    return result;
}
