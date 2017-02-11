#ifndef TRIP_H
#define TRIP_H

#include "station.h"
#include "qdatetime.h"

#include <QString>

namespace bss {
class Trip;
}

class Trip
{
public:  
    Trip(const Station& startStation = Station(),
         const Station& endStation = Station(),
         const QDateTime& startDateTime = QDateTime(),
         const QDateTime& endDateTime = QDateTime(),
         const QTime& duration = QTime());

    inline bool isValid() const { return m_isValid; }
    inline bool isNull() const { return m_isNull; }

    inline const Station& getStartStation() const { return m_startStation; }
    inline const Station& getEndStation() const { return m_endStation; }
    inline const QDateTime& getStartDateTime() const { return m_startDateTime; }
    inline const QDateTime& getEndDateTime() const { return m_endDateTime; }
    inline const QTime& getDuration() const { return m_duration; }
    inline qreal getDistance() const { return m_distance; }
    inline qreal getDirection() const { return m_direction; }

    static bool isValid(const Trip& t);
    static bool isNull(const Trip& t);
    static qreal calculateDistance(const Station& s1, const Station& s2);
    static qreal calculateDirection(const Station& s1, const Station& s2);

    operator QString() const;
    bool operator ==(const Trip& trip) const;

private:
    bool m_isValid;
    bool m_isNull;

    Station m_startStation;
    Station m_endStation;
    QDateTime m_startDateTime;
    QDateTime m_endDateTime;
    QTime m_duration;
    qreal m_distance;
    qreal m_direction;
};

#endif // TRIP_H
