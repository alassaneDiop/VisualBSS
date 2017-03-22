#ifndef STATIONSFILTER_H
#define STATIONSFILTER_H

#include <QVector>

#include "typedefs.h"


struct StationsFilterParams
{
    qreal minFlow = 0;
    qreal maxFlow = 1000000;
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
