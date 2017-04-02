#ifndef STATIONSFILTER_H
#define STATIONSFILTER_H

#include <QVector>



/**
 * @brief The StationsFilterParams struct contains data to filter stations.
 * More precisely, it contains an interval for the <i>tripsFlow</i>.
 */
struct StationsFilterParams
{
    int minTripsFlow = 0;   /// default value is <i>0</i>
    int maxTripsFlow = 0;   /// default value is <i>0</i>
};

struct Station;

/**
 * The StationsFilter class filters stations by their <i>tripsFlow</i> property.
 */
class StationsFilter
{
public:
    StationsFilter();
    StationsFilter(const StationsFilterParams& params);

    inline const StationsFilterParams& params() const { return m_params; }
    inline void setParams(const StationsFilterParams& params) { m_params = params; }

    /**
     * Filters the specified stations by their trips flow.
     * @param stations The stations to filter.
     * @return The filtered stations.
     */
    QVector<Station> filter(QVector<Station> stations) const;

private:
    StationsFilterParams m_params;
};

#endif // STATIONSFILTER_H
