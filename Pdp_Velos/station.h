#ifndef STATION_H
#define STATION_H

#include <QVector>
#include <QString>
#include <QTime>

#include "trip.h"

namespace bss {
struct StationData;
}


struct Station
{
    typedef unsigned short id_t;

    void appendArrival(const Trip& trip);
    void appendDeparture(const Trip& trip);
    void appendCycle(const Trip& trip);
    void updateAppend(const Trip& trip);

    qreal distance(const Station& to) const;
    qreal direction(const Station& to) const;
    bool operator==(const Station& other) const;
    QString toString() const;

    id_t id = (id_t)-1;
    QString name;
    qreal latitude = 0;
    qreal longitude = 0;
    quint64 avgTripDuration;
    qreal avgTripDistance = 0;
    qreal maxTripDistance = 0;
    int originDestinationFlow = 0;
    QVector<Trip::id_t> arrivalsId;
    QVector<Trip::id_t> departuresId;
    QVector<Trip::id_t> cyclesId;
};

#endif // STATION_H
