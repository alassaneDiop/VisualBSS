#ifndef STATIONSFILTER_H
#define STATIONSFILTER_H

#include <QVector>



/**
 * @brief The StationsFilterParams contains the parameters to filter stations.
 * Stations are filtered by their trips flow.
 */
struct StationsFilterParams
{
    qreal minTripsFlow = 0;
    qreal maxTripsFlow = 0;
};

struct Station;
class StationsFilter
{
public:
    StationsFilter();
    StationsFilter(const StationsFilterParams& params);

    inline const StationsFilterParams& params() const { return m_params; }
    inline void setParams(const StationsFilterParams& params) { m_params = params; }

    QVector<Station> filter(const QVector<Station>& stations) const;

private:
    StationsFilterParams m_params;
};

#endif // STATIONSFILTER_H
