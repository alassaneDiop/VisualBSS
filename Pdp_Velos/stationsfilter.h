#ifndef STATIONSFILTER_H
#define STATIONSFILTER_H

#include "station.h"
#include <QSet>

class StationsFilter
{
    struct FilterParams
    {
        qreal minOriginDestinationFlow = 0;
        qreal maxOriginDestinationFlow = 0;
    };

public:
    StationsFilter();
    StationsFilter(const FilterParams& params);

    inline const FilterParams& getparams() const { return m_params; }
    inline void setParams(const FilterParams& params) { m_params = params; }

    QSet<const Station*> filter(const QSet<const Station*>& stations) const;
    void filter(QSet<const Station*>& stations);

private:
    FilterParams m_params;
};

#endif // STATIONSFILTER_H
