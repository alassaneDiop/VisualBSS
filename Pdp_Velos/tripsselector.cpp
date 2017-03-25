#include "tripsselector.h"

#include <QtConcurrent>

#include "trip.h"


TripsSelector::TripsSelector(const TripsSelectionParams& params) :
    m_params(params)
{

}



QVector<Trip> TripsSelector::selectFrom(const QVector<Trip>& trips) const
{
    const auto select = [this](const Trip& t)
    {
        return (t.startDateTime.time().hour() >= params().fromHour)
            && (t.endDateTime.time().hour() <= params().toHour)
            && ((params().stationsIds.contains(t.startStationId))
            || (params().stationsIds.contains(t.endStationId)));
    };

    return QtConcurrent::blockingFiltered(trips, select);
}
