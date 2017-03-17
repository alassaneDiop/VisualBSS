#ifndef STATIONSSORTER_H
#define STATIONSSORTER_H

#include <QVector>

#include "typedefs.h"


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
    typedef bool(*greaterThanFctPtr)(const Station&, const Station&);

public:
    StationsSorter(const bss::SortOrder& sortParam);

    inline bss::SortOrder sortParam() const {return m_sortParam; }
    void setSortParam(const bss::SortOrder& param);

    QVector<Station> sort(const QVector<Station>& stations) const;
    void sort(QVector<Station>& stations) const;

private:
    bss::SortOrder m_sortParam;
    greaterThanFctPtr m_greaterThan;
};

#endif // STATIONSSORTER_H
