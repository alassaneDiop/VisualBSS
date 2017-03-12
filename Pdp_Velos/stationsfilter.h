#ifndef STATIONSFILTER_H
#define STATIONSFILTER_H

#include "station.h"
#include <QSet>


struct StationsFilterParams
{
    qreal minOriginDestinationFlow = 0;
    qreal maxOriginDestinationFlow = 0;
};


class StationsFilter
{
public:
    StationsFilter();
    StationsFilter(const StationsFilterParams& params);

    inline const StationsFilterParams& params() const { return m_params; }
    inline void setParams(const StationsFilterParams& params) { m_params = params; }

    QVector<bss::stationId> filter(const QVector<Station>& stations) const;

private:
    StationsFilterParams m_params;
};

#endif // STATIONSFILTER_H
