#include "tripsfilter.h"

#include <QtConcurrent>

#include "trip.h"


TripsFilter::TripsFilter(const TripsFilterParams& params) :
    m_params(params)
{

}

QVector<Trip> TripsFilter::filter(const QVector<Trip>& trips) const
{
    // TODO : SEB finir TripsFilter::filter
    const auto filter = [this](const Trip& t)
    {
        return (t.direction <= params().maxDirection)
            && (t.direction >= params().minDirection)
            && (t.distance <= params().maxDistance)
            && (t.distance >= params().minDistance)
            && (t.duration <= params().maxDuration)
            && (t.duration >= params().minDuration);
    };

    return QtConcurrent::blockingFiltered(trips, filter);
}
