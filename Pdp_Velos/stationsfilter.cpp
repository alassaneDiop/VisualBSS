#include "stationsfilter.h"
#include <QtConcurrent>


StationsFilter::StationsFilter()
{

}

StationsFilter::StationsFilter(const StationsFilterParams& params) :
    m_params(params)
{

}

QVector<bss::stationId> StationsFilter::filter(const QVector<Station>& stations) const
{
    QMutex lock;
    QVector<bss::stationId> ids;

    const auto filterFunctor = [this, &lock, &ids](const Station& s)
    {
        bool b = true;
        b &= (s.originDestinationFlow <= params().maxOriginDestinationFlow);
        b &= (s.originDestinationFlow >= params().minOriginDestinationFlow);
        if (b)
        {
            lock.lock();
            ids.append(s.id);
            lock.unlock();
        }
    };

    QtConcurrent::blockingMap(stations, filterFunctor);
    return ids;
}
