#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QDateTime>

namespace bss {
class Trip;
}

class Station;
class Trip
{
public:
    Trip();
    Trip(Station* const startStation,
         Station* const endStation,
         const QDateTime& startDateTime,
         const QDateTime& endDateTime);
    ~Trip();

    inline bool isValid() const { return m_isValid; }
    inline bool isNull() const { return m_isNull; }
    inline bool isCyclic() const { return m_isCyclic; }
    inline Station* getStartStation() const { return m_startStation; }
    inline Station* getEndStation() const { return m_endStation; }
    inline const QDateTime& getStartDateTime() const { return m_startDateTime; }
    inline const QDateTime& getEndDateTime() const { return m_endDateTime; }
    inline const QTime& getDuration() const { return m_duration; }
    inline qreal getDistance() const { return m_distance; }
    inline qreal getDirection() const { return m_direction; }

    QString toString() const;

private:
    bool m_isValid;
    bool m_isNull;
    bool m_isCyclic;
    Station* m_startStation;
    Station* m_endStation;
    QDateTime m_startDateTime;
    QDateTime m_endDateTime;
    QTime m_duration;
    qreal m_distance;
    qreal m_direction;
};

#endif // TRIP_H
