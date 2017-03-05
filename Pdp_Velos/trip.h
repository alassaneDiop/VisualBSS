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
    TripData(const int& id = -1,
             const int& startId = -1,
             const int& endId = -1) :
        id(id), startId(startId), endId(endId) { }
    int id;
    int startId;
    int endId;
    bool isCyclic = false;
    QDateTime startDateTime;
    QDateTime endDateTime;
    int durationMsec = 0;
    qreal direction = 0;
    qreal distance = 0;
};

class Trip
{
public:
    Trip();
    Trip(const TripData& data);

    inline int id() const { return m_id; }
    inline int startStationId() const { return m_startStationId; }
    inline int endStationId() const { return m_endStationId; }
    inline bool isCyclic() const { return m_isCyclic; }
    inline const QDateTime& startDateTime() const { return m_startDateTime; }
    inline const QDateTime& endDateTime() const { return m_endDateTime; }
    inline const QTime& duration() const { return m_duration; }
    inline qreal distance() const { return m_distance; }
    inline qreal direction() const { return m_direction; }

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
