#ifndef STATION_H
#define STATION_H

#include <QSet>
#include <QString>
#include <QTime>

namespace bss {
class Station;
}


class Trip;
class Station
{
public:
    Station();
    Station(const QString& name,
            const qreal latitude,
            const qreal longitude);

    inline bool isValid() const { return m_isValid; }
    inline bool isNull() const { return m_isNull; }
    inline QString getName() const { return m_name; }
    inline qreal getLatitude() const { return m_latitude; }
    inline qreal getLongitude() const { return m_longitude; }
    inline QTime getMinTripsDuration() const { return m_minTripsDuration; }
    inline QTime getAvgTripsDuration() const { return m_avgTripsDuration; }
    inline QTime getMaxTripsDuration() const { return m_maxTripsDuration; }
    inline qreal getMinTripsDistance() const { return m_minTripsDistance; }
    inline qreal getAvgTripsDistance() const { return m_avgTripsDistance; }
    inline qreal getMaxTripsDistance() const { return m_maxTripsDistance; }
    inline int getOriginDesinationFlow() const { return m_originDesinationFlow; }
    inline QList<const Trip*> getIncomingTrips() const { return m_incomingTrips.values(); }
    inline QList<const Trip*> getOutgoingTrips() const { return m_outgoingTrips.values(); }
    inline QList<const Trip*> getCyclicTrips() const { return m_cyclicTrips.values(); }

    QSet<const Trip*>::iterator insertIncomingTrip(const Trip* trip);
    QSet<const Trip*>::iterator insertOutgoingTrip(const Trip* trip);

    qreal distance(const Station* to) const;
    qreal direction(const Station* to) const;

    bool operator==(const Station& other) const;
    QString toString() const;

private:
    void onTripInserted(const Trip* trip);
    QTime calculateMinTripsDuration() const;
    QTime calculateAvgTripsDuration() const;
    QTime calculateMaxTripsDuration() const;
    qreal calculateMinTripsDistance() const;
    qreal calculateAvgTripsDistance() const;
    qreal calculateMaxTripsDistance() const;
    int calculateOriginDesinationFlow() const;

    bool m_isValid;
    bool m_isNull;
    QString m_name;
    qreal m_latitude;
    qreal m_longitude;
    QTime m_minTripsDuration;
    QTime m_avgTripsDuration;
    QTime m_maxTripsDuration;
    qreal m_minTripsDistance;
    qreal m_avgTripsDistance;
    qreal m_maxTripsDistance;
    int m_originDesinationFlow;
    QSet<const Trip*> m_incomingTrips;
    QSet<const Trip*> m_outgoingTrips;
    QSet<const Trip*> m_cyclicTrips;
};

#endif // STATION_H
