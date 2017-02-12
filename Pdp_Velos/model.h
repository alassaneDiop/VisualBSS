#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QSet>
#include "trip.h"
#include "iterator"

namespace bss {
class Model;
}

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    int loadData(const QString& filename);

    inline QList<Station> getStations() const { return m_stations.values(); }
    inline QList<Trip> getTrips() const { return m_trips.values(); }

    static QSet<Station> getStationsFromTrip(const Trip& trip);
    static QSet<Station> getStationsFromTrips(const QList<Trip>& trips);

private:
    QSet<Station> m_stations;
    QSet<Trip> m_trips;

signals:
    void dataLoaded(const QList<Station>& stations, const QList<Trip>& trips);

public slots:
};

#endif // MODEL_H
