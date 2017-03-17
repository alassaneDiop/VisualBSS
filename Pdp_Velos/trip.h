#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QDateTime>

#include "typedefs.h"

namespace bss {
struct Trip;
}


struct Trip
{
    inline bool operator ==(const Trip& other) { return (id == other.id); }
    static qreal calculateDistance(const Trip& trip);

    bss::tripId id = (bss::tripId) -1;
    bss::stationId startStationId = (bss::stationId) -1;
    bss::stationId endStationId = (bss::stationId) -1;
    bool isCyclic = false;
    quint64 duration = 0;
    qreal direction = 0;
    qreal distance = 0;
    QDateTime startDateTime;
    QDateTime endDateTime;
};

#endif // TRIP_H
