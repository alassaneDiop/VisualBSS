#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QDateTime>

namespace bss {
class Trip;
struct TripData;
}

struct TripData
{
    int id = -1;
    int startId = -1;
    int endId = -1;
    QDateTime startDateTime;
    QDateTime endDateTime;
    qreal direction = 0;
    qreal distance = 0;
};

class Trip
{
public:
    Trip();
    Trip(const TripData& data);

    inline int getId() const { return m_id; }
    inline int getStartStationId() const { return m_startStationId; }
    inline int getEndStationId() const { return m_endStationId; }
    inline bool isCyclic() const { return m_isCyclic; }
    inline const QDateTime& getStartDateTime() const { return m_startDateTime; }
    inline const QDateTime& getEndDateTime() const { return m_endDateTime; }
    inline const QTime& getDuration() const { return m_duration; }
    inline qreal getDistance() const { return m_distance; }
    inline qreal getDirection() const { return m_direction; }

    QString toString() const;

private:
    int m_id;
    int m_startStationId;
    int m_endStationId;
    bool m_isCyclic;
    QDateTime m_startDateTime;
    QDateTime m_endDateTime;
    QTime m_duration;
    qreal m_distance;
    qreal m_direction;
};

#endif // TRIP_H
