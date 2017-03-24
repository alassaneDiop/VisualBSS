#ifndef STATIONSSORTER_H
#define STATIONSSORTER_H

#include <QVector>


namespace bss {
class StationsSorter;
enum SortParam
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
    StationsSorter(const bss::SortParam& sortParam);

    inline bss::SortParam sortParam() const {return m_sortParam; }
    void setSortParam(const bss::SortParam& param);

    QVector<Station> sort(const QVector<Station>& stations) const;
    void sort(QVector<Station>& stations) const;

private:
    bss::SortParam m_sortParam;
    greaterThanFctPtr m_greaterThan;
};

#endif // STATIONSSORTER_H
