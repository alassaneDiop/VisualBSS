#include "tripsselector.h"

#include <QtConcurrent>

#include "trip.h"


TripsSelector::TripsSelector(const TripsSelectionParams& params) :
    m_params(params)
{

}



QVector<Trip> TripsSelector::selectFrom(const QVector<Trip>& trips) const
{
    const QTime fromTime = QTime(params().fromHour, 0, 0);
    const QTime toTime = QTime(params().toHour, 0, 0);

    const auto select = [this, &fromTime, &toTime](const Trip& t)
    {     
        return (t.startDateTime.time() >= fromTime)
            && (t.endDateTime.time() <= toTime)
            && ((params().stationsIds.contains(t.startStationId))
            || (params().stationsIds.contains(t.endStationId)));
    };

    return QtConcurrent::blockingFiltered(trips, select);
}
