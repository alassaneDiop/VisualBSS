#include "tripsfilter.h"

#include <QtConcurrent>

#include "trip.h"


TripsFilter::TripsFilter(const TripsFilterParams& params) :
    m_params(params)
{

}

QVector<Trip> TripsFilter::filter(const QVector<Trip>& trips) const
{
    const int day = params().day.isValid() ? params().day.day() : 1;
    const QDate date = QDate(params().period.year(),
                             params().period.month(),
                             day);

    const auto filter = [this, &date](const Trip& t)
    {
        return (t.direction <= params().maxDirection)
            && (t.direction >= params().minDirection)
            && (t.distance <= params().maxDistance)
            && (t.distance >= params().minDistance)
            && (t.duration <= params().maxDuration)
            && (t.duration >= params().minDuration)
            && (t.startDateTime.date() >= date);
    };

    return QtConcurrent::blockingFiltered(trips, filter);
}
