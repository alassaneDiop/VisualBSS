/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include "stationssorter.h"

#include <QtConcurrent>
#include <QtAlgorithms>

#include "station.h"
#include "data.h"

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

QVector<int> StationsSorter::sort(const QVector<int>& stationsIds, const Data& data) const
{
    QVector<Station> stations;
    stations.reserve(stationsIds.size());
    for (const int id : stationsIds)
        stations.append(data.station(id));

    stations = sort(stations);

    QVector<int> sortedStationsIds;
    sortedStationsIds.reserve(stations.size());
    for (const Station s : stations)
        sortedStationsIds.append(s.id);

    return sortedStationsIds;
}
