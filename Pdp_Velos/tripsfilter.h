#ifndef FILTER_H
#define FILTER_H

#include "typedefs.h"

#include <QDate>
#include <QVector>


namespace bss {
class TripsFilter;
}

/**
 * @brief The TripsFilterParams struct contains parameteres to filter trips.
 * Trips can be filtered by their dates, duration (in seconds),
 * distance (in meters) and direction (in degrees).
 * Distance, duration and direction parameters are set to 0 by default.
 */
struct TripsFilterParams
{
    QDate fromPeriod;
    QDate toPeriod;
    quint64 maxDuration = 0;    /// in seconds
    quint64 minDuration = 0;    /// in seconds
    qreal maxDistance = 0;      /// in meters
    qreal minDistance = 0;      /// in meters
    qreal maxDirection = 0;     /// in degrees
    qreal minDirection = 0;     /// in degrees
};


struct Trip;

/**
 * The TripsFilter class filters trips according to its filter parameters.
 * @see TripsFilterParams
 */
class TripsFilter
{  
public:
    TripsFilter(const TripsFilterParams& params);

    inline const TripsFilterParams& params() const { return m_params; }
    inline void setParams(const TripsFilterParams& params) { m_params = params; }

    /**
     * Filters an the specified trips set according to the filtering parameters.
     * @param trips The original trips set to filter from.
     * @return All the filters from the specified set that match the parameters.
     */
    QVector<Trip> filter(const QVector<Trip>& trips) const;

private:
    TripsFilterParams m_params;
};

#endif // FILTER_H
