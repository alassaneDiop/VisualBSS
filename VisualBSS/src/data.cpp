#include "data.h"
#include "datafilereader.h"

Data::Data(QObject* parent) :
    QObject(parent)
{

}

QVector<Station> Data::stations(const QVector<int>& ids) const
{
    QVector<Station> stations;
    stations.reserve(ids.size());
    for (const int id : ids)
        stations.append(station(id));

    return stations;
}

QVector<Trip> Data::trips(const QVector<int>& ids) const
{
    QVector<Trip> trips;
    trips.reserve(ids.size());
    for (const int id : ids)
        trips.append(trip(id));

    return trips;
}

bool Data::load(const QString& filename, const bool& parallel)
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

bool Data::clear()
{
    if (!hasData())
    {
        emit failedToClearData();
        return false;
    }
    else
    {
        m_trips.clear();
        m_trips.squeeze();

        m_stations.clear();
        m_stations.squeeze();

        m_hasData = false;
        emit dataCleared();
        return true;
    }
}
