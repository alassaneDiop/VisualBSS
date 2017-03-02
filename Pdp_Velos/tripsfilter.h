#ifndef FILTER_H
#define FILTER_H

#include <QDate>
#include <QSet>
#include <QTime>

namespace bss {
class TripsFilter;
}

class Trip;
class TripsFilter
{  
    struct FilterParams
    {
        QDate period;
        QDate day;
        Qt::DayOfWeek dayOfWeek = Qt::DayOfWeek::Monday;
        bool showArrivals = true;
        bool showDepartures = true;
        bool showCycles = true;
        bool showDistance = true;
        QTime maxDuration;
        QTime minDuration;
        qreal maxDistance = 0;
        qreal minDistance = 0;
        qreal maxDirection = 0;
        qreal minDirection = 0;
    };

public:
    TripsFilter();
    TripsFilter(const FilterParams& params);

    inline const FilterParams& getparams() const { return m_params; }
    inline void setParams(const FilterParams& params) { m_params = params; }

    QSet<const Trip*> filter(const QSet<const Trip*>& trips) const;
    void filter(QSet<const Trip*>& trips);

private:
    FilterParams m_params;
};

#endif // FILTER_H
