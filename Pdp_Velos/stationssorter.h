#ifndef STATIONSSORTER_H
#define STATIONSSORTER_H

#include "typedefs.h"

#include <QVector>


namespace bss {
class StationsSorter;
enum SortOrder
{
    DEPARTURES,
    ARRIVALS,
    CYCLES,
    DISTANCE,
    DURATION
};
}

struct Station;
class StationsSorter
{
public:
    StationsSorter();
    StationsSorter(const bss::SortOrder& sortParam);

    inline bss::SortOrder sortParam() const {return m_sortParam; }
    inline void setSortParam(const bss::SortOrder& param) { m_sortParam = param; }

    QVector<bss::stationId> sort(const QVector<Station>& stations) const;

private:
    bss::SortOrder m_sortParam;
};

#endif // STATIONSSORTER_H
