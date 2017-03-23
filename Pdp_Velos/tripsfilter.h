#ifndef FILTER_H
#define FILTER_H

#include "typedefs.h"

#include <QDate>
#include <QVector>


namespace bss {
class TripsFilter;
}


struct TripsFilterParams
{
    QDate fromPeriod;
    QDate toPeriod;
    Qt::DayOfWeek dayOfWeek = Qt::DayOfWeek::Monday;
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
    TripsFilter(const TripsFilterParams& params);

    inline const TripsFilterParams& params() const { return m_params; }
    inline void setParams(const TripsFilterParams& params) { m_params = params; }

    QVector<Trip> filter(const QVector<Trip>& trips) const;

private:
    TripsFilterParams m_params;
};

#endif // FILTER_H
