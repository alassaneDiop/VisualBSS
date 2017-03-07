#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QDateTime>

namespace bss {
struct TripData;
}

struct Trip
{
    QString toString() const;

    int id = -1;
    int startStationId = -1;
    int endStationId = -1;
    bool isCyclic = false;
    QDateTime startDateTime;
    QDateTime endDateTime;
    QTime duration;
    qreal direction = 0;

    qreal distance = 0;

    static qreal calculateDistance(const Trip& trip);
};

#endif // TRIP_H
