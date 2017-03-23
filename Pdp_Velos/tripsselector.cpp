#include "tripsselector.h"

#include <QtConcurrent>

#include "trip.h"


TripsSelector::TripsSelector(const TripsSelectionParams& params) :
    m_params(params)
{

}



QVector<Trip> TripsSelector::select(const QVector<Trip>& trips) const
{
    const auto select = [this](const Trip& t)
    {
        return (t.startDateTime.time().hour() >= params().fromHour)
            && (t.endDateTime.time().hour() <= params().toHour)
            && ((params().stations.contains(t.startStationId))
            || (params().stations.contains(t.endStationId)));
    };

    return QtConcurrent::blockingFiltered(trips, select);
}
