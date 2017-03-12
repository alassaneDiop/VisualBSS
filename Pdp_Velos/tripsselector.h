#ifndef SELECTOR_H
#define SELECTOR_H

#include "typedefs.h"
#include <QVector>


namespace bss {
class TripsSelector;
}


struct TripsSelectionParams
{
    int startHour = -1;
    int endHour = -1;
    QVector<bss::stationId> stations;
};


struct Station;
struct Trip;
class TripsSelector
{
public:
    TripsSelector();
    TripsSelector(const TripsSelectionParams& params);

    inline const TripsSelectionParams& params() const { return m_params; }
    inline void setParams(const TripsSelectionParams& params) { m_params = params; }

    QVector<bss::tripId> selectTripsFrom(const QVector<Trip>& trips) const;

private:
    TripsSelectionParams m_params;
};

#endif // SELECTOR_H
