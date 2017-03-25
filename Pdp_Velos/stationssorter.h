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

/**
 * @brief The StationsSorter class sorts stations by a specified property.
 */
class StationsSorter
{
    typedef bool(*greaterThanFctPtr)(const Station&, const Station&);

public:
    StationsSorter(const bss::SortParam& sortParam);

    inline bss::SortParam sortParam() const {return m_sortParam; }
    void setSortParam(const bss::SortParam& param);

    /**
     * @brief sort Sorts the specified stations by a parameter.
     * @param stations The stations to sort.
     * @return The sorted stations.
     */
    QVector<Station> sort(const QVector<Station>& stations) const;

    /**
     * @brief sort Sorts the specified stations by a parameter.
     * @param stations The stations to sort.
     */
    void sort(QVector<Station>& stations) const;

private:
    bss::SortParam m_sortParam;
    greaterThanFctPtr m_greaterThan;
};

#endif // STATIONSSORTER_H
