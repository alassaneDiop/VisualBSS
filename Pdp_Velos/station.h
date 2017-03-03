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
    inline QTime getAvgTripsDuration() const { return m_avgTripsDuration; }
    inline qreal getAvgTripsDistance() const { return m_avgTripsDistance; }
    inline qreal getMaxTripsDistance() const { return m_maxTripsDistance; }
    inline int getOriginDesinationFlow() const { return m_originDesinationFlow; }

    inline int getArrivalsCount() const { return m_arrivals.size(); }
    inline int getDeparturesCount() const { return m_departures.size(); }
    inline int getCyclesCount() const { return m_cycles.size(); }

    inline QList<const Trip*> getIncomingTrips() const { return m_arrivals.values(); }
    inline QList<const Trip*> getOutgoingTrips() const { return m_departures.values(); }
    inline QList<const Trip*> getCyclicTrips() const { return m_cycles.values(); }

    const QSet<const Trip*>::iterator insertArrival(const Trip* trip);
    const QSet<const Trip*>::iterator insertDeparture(const Trip* trip);
    const QSet<const Trip*>::iterator insertCycle(const Trip* trip);

    qreal distance(const Station* to) const;
    qreal direction(const Station* to) const;

    bool operator==(const Station& other) const;
    QString toString() const;

private:
    void onTripInserted(const Trip* trip);

    bool m_isValid;
    bool m_isNull;
    QString m_name;
    qreal m_latitude;
    qreal m_longitude;
    QTime m_avgTripsDuration;
    qreal m_avgTripsDistance = 0;
    qreal m_maxTripsDistance = 0;
    int m_originDesinationFlow = 0;
    QSet<const Trip*> m_arrivals;
    QSet<const Trip*> m_departures;
    QSet<const Trip*> m_cycles;
};

#endif // STATION_H
