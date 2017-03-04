#ifndef STATIONSSORTER_H
#define STATIONSSORTER_H

#include <QList>
#include <QSet>

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

class Station;
class StationsSorter
{
public:
    StationsSorter(const bss::SortOrder& sortParam);

    inline bss::SortOrder getSortParam() const {return m_sortParam; }
    inline void setSortParam(const bss::SortOrder& param) { m_sortParam = param; }

    QList<const Station*> sort(const QSet<const Station*>& stations) const;
    void sort(QSet<const Station*>& stations) const;

private:
    bss::SortOrder m_sortParam;
};

#endif // STATIONSSORTER_H
