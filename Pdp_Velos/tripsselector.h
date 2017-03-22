#ifndef SELECTOR_H
#define SELECTOR_H

#include <QVector>

#include "typedefs.h"


namespace bss {
class TripsSelector;
}


struct TripsSelectionParams
{
    int startHour = -1;
    int endHour = -1;
    QVector<bss::stationId> stations;
};


struct Trip;
class TripsSelector
{
public:
    TripsSelector(const TripsSelectionParams& params);

    inline const TripsSelectionParams& params() const { return m_params; }
    inline void setParams(const TripsSelectionParams& params) { m_params = params; }

    QVector<Trip> select(const QVector<Trip>& trips) const;

private:
    TripsSelectionParams m_params;
};

#endif // SELECTOR_H
