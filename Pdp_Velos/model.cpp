#include "model.h"
#include "datafilereader.h"

Model::Model(QObject* parent) :
    QObject(parent)
{

}

QVector<Station> Model::stations(const QVector<bss::stationId>& ids) const
{
    QVector<Station> stations;
    stations.reserve(ids.size());
    for (const bss::stationId id : ids)
        stations.append(station(id));

    return stations;
}

QVector<Trip> Model::trips(const QVector<bss::tripId>& ids) const
{
    QVector<Trip> trips;
    trips.reserve(ids.size());
    for (const bss::tripId id : ids)
        trips.append(trip(id));

    return trips;
}

bool Model::loadData(const QString& filename, const bool& parallel)
{
    QHash<QString, Station> stationsHash;
    stationsHash.reserve(m_stations.size());
    for (const Station s : m_stations)
        stationsHash.insert(s.name, s);

    const DataFileReader dataFileReader(filename);
    DataFileReadInfo info;
    if (!parallel)
        info = dataFileReader.readData(stationsHash, m_trips);
    else
    {
        QReadWriteLock stationsLock, tripsLock;
        info = dataFileReader.parallelReadData(stationsHash, m_trips, stationsLock, tripsLock);
    }

    if (!info.ok)
    {
        emit failedToLoadData(filename, info.errorString);
        return false;
    }
    else
    {
        m_hasData = true;
        m_stations = stationsHash.values().toVector();
        emit dataLoaded(m_trips, m_stations);
        return true;
    }
}

bool Model::unloadData()
{
    if (!hasData())
    {
        emit failedToUnloadData();
        return false;
    }
    else
    {
        m_trips.clear();
        m_trips.squeeze();

        m_stations.clear();
        m_stations.squeeze();

        m_hasData = false;
        emit dataUnloaded();
        return true;
    }
}
