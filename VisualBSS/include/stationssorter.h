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
class Data;

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
    QVector<Station> sort(QVector<Station> stations) const;

    /**
     * @brief sort Sorts the specified stations ids by a parameter.
     * @param ids The ids of the stations to sort.
     * @param data The object that contains all the stations.
     * @return The ids of the sorted stations.
     */
    QVector<int> sort(const QVector<int>& stationsIds, const Data& data) const;

private:
    bss::SortParam m_sortParam;
    greaterThanFctPtr m_greaterThan;
};

#endif // STATIONSSORTER_H
