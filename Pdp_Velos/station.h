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
    StationData(const int& id = -1,
                const QString& name = QString(),
                const qreal& latitude = 0,
                const qreal& longitude = 0) :
        id(id), name(name), latitude(latitude), longitude(longitude) { }

    int id = -1;
    QString name;
    qreal latitude = 0;
    qreal longitude = 0;
    int avgTripDurationMsec = 0;
    qreal avgTripDistance = 0;
    qreal maxTripDistance = 0;
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

    inline int id() const { return m_id; }
    inline QString name() const { return m_name; }
    inline qreal latitude() const { return m_latitude; }
    inline qreal longitude() const { return m_longitude; }
    inline QTime avgTripDuration() const { return m_avgTripDuration; }
    inline qreal avgTripDistance() const { return m_avgTripDistance; }
    inline qreal maxTripDistance() const { return m_maxTripsDistance; }
    inline int originDesinationFlow() const { return m_originDesinationFlow; }

    inline int arrivalsCount() const { return m_arrivalsId.size(); }
    inline int departuresCount() const { return m_departuresId.size(); }
    inline int cyclesCount() const { return m_cyclesId.size(); }

    inline int arrival(const int& i) const { return m_arrivalsId.at(i); }
    inline int departure(const int& i) const { return m_departuresId.at(i); }
    inline int cycle(const int& i) const { return m_cyclesId.at(i); }

    qreal distance(const Station& to) const;
    qreal direction(const Station& to) const;

    bool operator==(const Station& other) const;
    QString toString() const;

private:
    int m_id;
    QString m_name;
    qreal m_latitude;
    qreal m_longitude;
    QTime m_avgTripDuration;
    qreal m_avgTripDistance;
    qreal m_maxTripsDistance;
    int m_originDesinationFlow;
    QVector<int> m_arrivalsId;
    QVector<int> m_departuresId;
    QVector<int> m_cyclesId;
};

#endif // STATION_H
