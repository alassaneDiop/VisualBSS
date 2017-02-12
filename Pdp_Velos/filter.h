//#ifndef FILTER_H
//#define FILTER_H

//#include <QObject>
//#include "station.h"

//#include "trip.h"
//#include <QDate>
//#include <QTime>
//#include <QSet>

//namespace bss {
//class Filter;
//enum Order
//{
//    INCOMING_TRIPS,
//    OUTGOING_TRIPS,
//    CYCLIC_TRIPS,
//    DISTANCE
//};
//}



//class Filter : public QObject
//{
//    Q_OBJECT
//public:
//    explicit Filter(QObject* parent = 0);

//    QSet<const Trip*> apply(const QSet<const Trip*> data) const;

//    inline QDate getDate() const { return date; }
//    inline bool getShowIncomingTrips() const { return showIncomingTrips; }
//    inline bool getShowOutgoingTrips() const { return showOutgoingTrips; }
//    inline bool getShowCyclicTrips() const { return showCyclicTrips; }
//    inline bool getShowTripsDistances() const { return showTripsDistances; }
//    inline bss::Order getOrder() const { return order; }
//    inline QTime getMaxDuration() const { return maxDuration; }
//    inline QTime getMinDuration() const { return minDuration; }
//    inline qreal getMaxDistance() const { return maxDistance; }
//    inline qreal getMinDistance() const { return minDistance; }
//    inline qreal getMaxDirection() const { return maxDirection; }
//    inline qreal getMinDirection() const { return minDirection; }

//    bool setDate(const QDate& date);
//    bool setShowIncomingTrips(const bool showIncomingTrips);
//    bool setShowOutgoingTrips(const bool showOutgoingTrips);
//    bool setShowCyclicTrips(const bool showCyclicTrips);
//    bool setShowTripsDistances(const bool showTripsDistances);
//    bool setOrder(const bss::Order order);
//    bool setMaxDuration(QTime maxDuration);
//    bool setMinDuration(QTime minDuration);
//    bool setMaxDistance(qreal maxDistance);
//    bool setMinDistance(qreal minDistance);
//    bool setMaxDirection(qreal maxDirection);
//    bool setMinDirection(qreal minDirection);

//private:
//    QDate date;
//    bool showIncomingTrips;
//    bool showOutgoingTrips;
//    bool showCyclicTrips;
//    bool showTripsDistances;
//    bss::Order order;
//    QTime maxDuration;
//    QTime minDuration;
//    qreal maxDistance;
//    qreal minDistance;
//    qreal maxDirection;
//    qreal minDirection;

//signals:
//    void changed();

//public slots:
//};

//#endif // FILTER_H
