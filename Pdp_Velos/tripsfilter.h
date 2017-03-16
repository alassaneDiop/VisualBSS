#ifndef FILTER_H
#define FILTER_H

#include "typedefs.h"

#include <QVector>
#include <QDate>


namespace bss {
class TripsFilter;
}


struct TripsFilterParams
{
    QDate period;
    QDate day;
    Qt::DayOfWeek dayOfWeek = Qt::DayOfWeek::Monday;
    bool shouldShowArrivals = true;
    bool shouldShowDepartures = true;
    bool shouldShowCycles = true;
    bool shouldShowDistance = true;
    quint64 maxDuration = 0;
    quint64 minDuration = 0;
    qreal maxDistance = 0;
    qreal minDistance = 0;
    qreal maxDirection = 0;
    qreal minDirection = 0;
};


struct Trip;
class TripsFilter
{  
public:
    TripsFilter();
    TripsFilter(const TripsFilterParams& params);

    inline const TripsFilterParams& params() const { return m_params; }
    inline void setParams(const TripsFilterParams& params) { m_params = params; }

    QVector<bss::tripId> filter(const QVector<Trip>& trips) const;

private:
    TripsFilterParams m_params;
};

#endif // FILTER_H
