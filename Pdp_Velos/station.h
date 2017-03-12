#ifndef STATION_H
#define STATION_H

#include "typedefs.h"

#include <QVector>
#include <QString>
#include <QTime>

namespace bss {
struct Station;
}

struct Trip;
struct Station
{
    inline bool operator ==(const Station& other) { return (id == other.id); }
    qreal distance(const Station& to) const;
    qreal direction(const Station& to) const;

    void appendArrival(const Trip& trip);
    void appendDeparture(const Trip& trip);
    void appendCycle(const Trip& trip);
    void updateAppend(const Trip& trip);

    bss::tripId id = (bss::tripId) -1;
    QString name;
    qreal latitude = 0;
    qreal longitude = 0;
    quint64 avgTripDuration;
    qreal avgTripDistance = 0;
    qreal maxTripDistance = 0;
    int originDestinationFlow = 0;
    QVector<bss::tripId> arrivalsId;
    QVector<bss::tripId> departuresId;
    QVector<bss::tripId> cyclesId;
};

#endif // STATION_H
