#include "trip.h"

QString Trip::toString() const
{
    QString result;
    result += "start station id :\t" + QString::number(startStationId) + '\n';
    result += "end station id :\t" + QString::number(endStationId) + '\n';
    result += "start time :\t" + startDateTime.toString() + '\n';
    result += "end time :\t" + endDateTime.toString() + '\n';
    result += "duration :\t" + QString::number(duration);
    return result;
}

qreal Trip::calculateDistance(const Trip& trip)
{
    // TODO : "For cyclic trips, we estimated the distance by multiplying the duration by the average speed of 2.7m/s."
    return 0;
}
