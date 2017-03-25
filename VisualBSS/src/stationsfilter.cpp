#include "stationsfilter.h"

#include <QtConcurrent>

#include "station.h"


StationsFilter::StationsFilter(const StationsFilterParams& params) :
    m_params(params)
{

}

QVector<Station> StationsFilter::filter(const QVector<Station>& stations) const
{
    const auto filter = [this](const Station& s)
    {
        return (s.tripsFlow <= params().maxTripsFlow)
            && (s.tripsFlow >= params().minTripsFlow);
    };

    return QtConcurrent::blockingFiltered(stations, filter);
}
