#include "model.h"


DataLoadResult Model::loadData(const QString& filename)
{
    QHash<QString, Station> stationsHash;
    for (const Station s : m_stations)
        stationsHash.insert(s.name, s);

    const DataFileReader dataFileReader = DataFileReader(filename);
    const DataFileReadInfo info = dataFileReader.readData(stationsHash, m_trips);

    DataLoadResult result;
    result.info = info;

    if (info.ok)
    {
        m_hasData = true;
        m_stations = stationsHash.values().toVector();
        result.trips = m_trips;
        result.stations = m_stations;
    }

    return result;
}

bool Model::unloadData()
{
    if (!hasData())
        return false;
    else
    {
        m_trips.clear();
        m_trips.squeeze();

        m_stations.clear();
        m_stations.squeeze();

        m_hasData = false;
        return true;
    }
}
