#ifndef TRIP_H
#define TRIP_H

#include <QDateTime>
#include <QtConcurrent>

namespace bss {
struct Trip;
}


/**
 * @brief The Trip struct contains all the data for its filtering and rendering.
 * A trip is identified by its id. It is associated to exactly two stations
 * by their ids (one for the departure, the other one for the arrival).
 * It is said cyclic if its start and end stations are the same.
 */
struct Trip
{
    inline bool operator ==(const Trip& other) { return (id == other.id); }

    int id =  -1;               /// Trip unique id (<i>-1</i by default).

    int startStationId = -1;    /// <i>-1</i by default.
    int endStationId = -1;      /// <i>-1</i by default.

    bool isCyclic = false;      /// <i>false</i> by default.

    qreal direction = 0;        /// Azimuth (in degrees, in range 0..360) from start station to end station.
    qint64 duration = 0;        /// Duration in seconds (<i>0</i> by default).
    qint64 distance = 0;        /// Distance in meters (<i>0</i> by default).

    QDateTime startDateTime;
    QDateTime endDateTime;
};

#endif // TRIP_H
