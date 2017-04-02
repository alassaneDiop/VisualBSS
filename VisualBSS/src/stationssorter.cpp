#include "stationssorter.h"

#include <QtConcurrent>
#include <QtAlgorithms>

#include "station.h"


StationsSorter::StationsSorter(const bss::SortParam& param) :
    m_sortParam((bss::SortParam) -1)
{
    setSortParam(param);
}

void StationsSorter::setSortParam(const bss::SortParam& param)
{
    if (m_sortParam != param)
    {
        m_sortParam = param;
        switch (param)
        {
        case bss::ARRIVALS:
            m_greaterThan = [](const Station& s1, const Station& s2)
            { return (s1.arrivalsIds.count() > s2.arrivalsIds.count()); };
            break;

        case bss::CYCLES:
            m_greaterThan = [](const Station& s1, const Station& s2)
            { return (s1.cyclesIds.count() > s2.cyclesIds.count()); };
            break;

        case bss::DEPARTURES:
            m_greaterThan = [](const Station& s1, const Station& s2)
            { return (s1.departuresIds.count() > s2.departuresIds.count()); };
            break;

        case bss::DISTANCE:
            m_greaterThan = [](const Station& s1, const Station& s2)
            { return (s1.avgTripDistance > s2.avgTripDistance); };
            break;

        case bss::DURATION:
            m_greaterThan = [](const Station& s1, const Station& s2)
            { return (s1.avgTripDuration > s2.avgTripDuration); };
            break;
        default:
            m_greaterThan = nullptr;
            break;
        }
    }
}

QVector<Station> StationsSorter::sort(QVector<Station> stations) const
{
    // Qt recommends to use the std function instead of its own, and include QtAlgorithms
    std::stable_sort(stations.begin(), stations.end(), m_greaterThan);
    return stations;
}
