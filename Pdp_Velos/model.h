#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>

#include "trip.h"
#include "station.h"
#include "typedefs.h"

namespace bss {
class Model;
}

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    inline int tripsCount() const { return m_trips.count(); }
    inline int stationsCount() const { return m_stations.count(); }

    inline Trip trip(const int& id) const { return m_trips[id]; }
    inline Station station(const int& id) const { return m_stations[id]; }

    inline const Trip& constTrip(const int& id) const { return m_trips.at(id); }
    inline const Station& constStation(const int& id) const { return m_stations.at(id); }

    inline QVector<Trip> trips() const { return QVector<Trip>(m_trips); }
    inline QVector<Station> stations() const { return QVector<Station>(m_stations); }

    inline const QVector<Trip>& constTrips() const { return m_trips; }
    inline const QVector<Station>& constStations() const { return m_stations; }

    int loadData(const QString& filename);

private:
    QVector<Trip> m_trips;
    QVector<Station> m_stations;

signals:
    void dataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void failedToLoadData(const QString& filename);

public slots:

};

#endif // MODEL_H
