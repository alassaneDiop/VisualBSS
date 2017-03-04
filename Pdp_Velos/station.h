#ifndef STATION_H
#define STATION_H

#include <QVector>
#include <QString>
#include <QTime>

namespace bss {
class Station;
struct StationData;
}

struct StationData
{
    int id = -1;
    QString name;
    qreal latitude = 0;
    qreal longitude = 0;
    QTime avgTripsDuration;
    qreal avgTripsDistance = 0;
    qreal maxTripsDistance = 0;
    int originDesinationFlow = 0;
    QVector<int> arrivalsId;
    QVector<int> departuresId;
    QVector<int> cyclesId;
};

class Station
{
public:
    Station();
    Station(const StationData& data);

    inline int getId() const { return m_id; }
    inline QString getName() const { return m_name; }
    inline qreal getLatitude() const { return m_latitude; }
    inline qreal getLongitude() const { return m_longitude; }
    inline QTime getAvgTripsDuration() const { return m_avgTripsDuration; }
    inline qreal getAvgTripsDistance() const { return m_avgTripsDistance; }
    inline qreal getMaxTripsDistance() const { return m_maxTripsDistance; }
    inline int getOriginDesinationFlow() const { return m_originDesinationFlow; }

    inline int getArrivalsCount() const { return m_arrivalsId.size(); }
    inline int getDeparturesCount() const { return m_departuresId.size(); }
    inline int getCyclesCount() const { return m_cyclesId.size(); }

    inline QVector<int> getArrivals() const { return QVector<int>(m_arrivalsId); }
    inline QVector<int> getDepartures() const { return QVector<int>(m_departuresId); }
    inline QVector<int> getCycles() const { return QVector<int>(m_cyclesId); }

    qreal distance(const Station& to) const;
    qreal direction(const Station& to) const;

    bool operator==(const Station& other) const;
    QString toString() const;

private:
    int m_id;
    QString m_name;
    qreal m_latitude;
    qreal m_longitude;
    QTime m_avgTripsDuration;
    qreal m_avgTripsDistance;
    qreal m_maxTripsDistance;
    int m_originDesinationFlow;
    QVector<int> m_arrivalsId;
    QVector<int> m_departuresId;
    QVector<int> m_cyclesId;
};

#endif // STATION_H
