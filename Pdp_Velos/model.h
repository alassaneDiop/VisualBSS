#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>

#include "trip.h"
#include "station.h"

namespace bss {
class Model;
}

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    inline int tripsCount() const { return m_tripsCount; }
    inline int stationsCount() const { return m_stationsCount; }

    inline Trip trip(const int& id) const { return m_trips[id]; }
    inline Station station(const int& id) const { return m_stations[id]; }

    inline const Trip& constTrip(const int& id) const { return m_trips.at(id); }
    inline const Station& constStation(const int& id) const { return m_stations.at(id); }

    int loadData(const QString& filename);

private:
    int m_tripsCount = 0;
    int m_stationsCount = 0;
    QVector<Trip> m_trips;
    QVector<Station> m_stations;

signals:
    void dataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void failedToLoadData(const QString& filename);

public slots:

};

#endif // MODEL_H
