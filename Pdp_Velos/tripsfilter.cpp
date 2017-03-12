#include "tripsfilter.h"
#include "trip.h"
#include <QtConcurrent>

TripsFilter::TripsFilter()
{

}

TripsFilter::TripsFilter(const TripsFilterParams& params) :
    m_params(params)
{

}

QVector<bss::tripId> TripsFilter::filter(const QVector<Trip>& trips) const
{
    // TODO : finir TripsFilter::filter
    QMutex lock;
    QVector<bss::tripId> ids;

    const auto filterFunctor = [this, &lock, &ids](const Trip& t)
    {
        bool b = true;
        b &= (t.direction <= params().maxDirection) && (t.direction >= params().minDirection);
        b &= (t.distance <= params().maxDistance) && (t.distance >= params().minDistance);
        b &= (t.duration <= params().maxDuration) && (t.duration >= params().minDuration);
        if (b)
        {
            lock.lock();
            ids.append(t.id);
            lock.unlock();
        }
    };

    QtConcurrent::blockingMap(trips, filterFunctor);
    return ids;
}
