#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>

namespace bss {
class Model;
}

#include "trip.h"
#include "station.h"

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    /*inline Trip getTrip(const int& i) const { return m_stations.value(i); }
    inline Station getStation(const int& i) const { return m_stations.value(i); }*/
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
