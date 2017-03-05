#include "model.h"
#include "datafilereader.h"
#include "csvdatafilereader.h"

Model::Model(QObject* parent) :
    QObject(parent)
{

}

int Model::loadData(const QString& filename)
{
    QVector<TripData> tripsData;
    QVector<StationData> stationsData;
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename);
    const bool ok = dataFileReader->readData(tripsData, stationsData);
    delete dataFileReader;

    if (!ok)
    {
        emit failedToLoadData(filename);
        return -1;
    }
    else
    {
        m_trips.clear();
        m_trips.reserve(tripsData.size());
        for (const TripData data : tripsData)
            m_trips.append(Trip(data));

        m_stations.clear();
        m_stations.reserve(stationsData.size());
        for (const StationData data : stationsData)
            m_stations.append(StationData(data));

        emit dataLoaded(m_trips, m_stations);
        return m_trips.size();
    }
}
