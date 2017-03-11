#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QDateTime>

namespace bss {
struct TripData;
}

struct Trip
{
    typedef unsigned int id_t;

    QString toString() const;

    id_t id = (id_t)-1;
    int startStationId = -1;
    int endStationId = -1;
    bool isCyclic = false;
    QDateTime startDateTime;
    QDateTime endDateTime;
    quint64 duration = 0;
    qreal direction = 0;
    qreal distance = 0;

    static qreal calculateDistance(const Trip& trip);
};

#endif // TRIP_H
