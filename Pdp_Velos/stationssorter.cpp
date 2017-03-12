#include "stationssorter.h"
#include "station.h"
#include <QtConcurrent>

StationsSorter::StationsSorter()
{

}

StationsSorter::StationsSorter(const bss::SortOrder& param) :
    m_sortParam(param)
{

}

QVector<bss::stationId> StationsSorter::sort(const QVector<Station>& stations) const
{
    typedef bool(*greaterThanFctPtr)(const Station&, const Station&);
    greaterThanFctPtr greaterThan;

    switch (sortParam())
    {
    case bss::ARRIVALS:
        greaterThan = [](const Station& s1, const Station& s2)
        { return (s1.arrivalsId.count() > s2.arrivalsId.count()); };
        break;

    case bss::CYCLES:
        greaterThan = [](const Station& s1, const Station& s2)
        { return (s1.cyclesId.count() > s2.cyclesId.count()); };
        break;

    case bss::DEPARTURES:
        greaterThan = [](const Station& s1, const Station& s2)
        { return (s1.departuresId.count() > s2.departuresId.count()); };
        break;

    case bss::DISTANCE:
        greaterThan = [](const Station& s1, const Station& s2)
        { return (s1.avgTripDistance > s2.avgTripDistance); };
        break;

    case bss::DURATION:
        greaterThan = [](const Station& s1, const Station& s2)
        { return (s1.avgTripDuration > s2.avgTripDuration); };
        break;
    }

    QVector<Station> sortedStations = QVector<Station>(stations);
    std::sort(sortedStations.begin(), sortedStations.end(), greaterThan);

    QVector<bss::stationId> sortedIds;
    sortedIds.reserve(sortedStations.size());

    for (const Station s : sortedStations)
        sortedIds.append(s.id);

    return sortedIds;
}
