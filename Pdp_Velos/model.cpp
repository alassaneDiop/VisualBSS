#include "model.h"
#include "datafilereader.h"
#include "csvdatafilereader.h"

Model::Model(QObject* parent) :
    QObject(parent)
{

}

int Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename, Qt::ISODate);
    const bool ok = dataFileReader->readData(m_trips, m_stations);
    delete dataFileReader;

    if (!ok)
    {
        emit failedToLoadData(filename);
        return -1;
    }
    else
    {
        m_tripsCount = m_trips.size();
        m_stationsCount = m_stations.size();
        emit dataLoaded(m_trips, m_stations);
        return m_trips.size();
    }
}
