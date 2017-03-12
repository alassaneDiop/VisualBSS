#include "tripsselector.h"
#include "trip.h"
#include <QtConcurrent>

TripsSelector::TripsSelector()
{

}

TripsSelector::TripsSelector(const TripsSelectionParams& params) :
    m_params(params)
{

}



QVector<bss::tripId> TripsSelector::selectTripsFrom(const QVector<Trip>& trips) const
{
    QMutex lock;
    QVector<bss::tripId> ids;

    const auto selectFunctor = [this, &lock, &ids](const Trip& t)
    {
        bool b = true;
        b &= t.startDateTime.time().hour() >= params().startHour;
        b &= t.endDateTime.time().hour() <= params().endHour;
        b |= params().stations.contains(t.startStationId);
        b |= params().stations.contains(t.endStationId);
        if (b)
        {
            lock.lock();
            ids.append(t.id);
            lock.unlock();
        }
    };

    QtConcurrent::blockingMap(trips, selectFunctor);
    return ids;
}
