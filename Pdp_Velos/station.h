#ifndef STATION_H
#define STATION_H

#include <QVector>
#include <QString>
#include <QTime>

namespace bss {
struct StationData;
}

struct Trip;
struct Station
{
    void appendArrival(const Trip& tripId);
    void appendDeparture(const Trip& tripId);
    void appendCycle(const Trip& tripId);
    void updateAppend(const Trip& tripId);

    qreal distance(const Station& to) const;
    qreal direction(const Station& to) const;
    bool operator==(const Station& other) const;
    QString toString() const;

    int id = -1;
    QString name;
    qreal latitude = 0;
    qreal longitude = 0;
    QTime avgTripDuration;
    qreal avgTripDistance = 0;
    qreal maxTripDistance = 0;
    int originDestinationFlow = 0;
    QVector<int> arrivalsId;
    QVector<int> departuresId;
    QVector<int> cyclesId;
};

#endif // STATION_H
