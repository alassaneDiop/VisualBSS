#include "model.h"
#include "datafilereader.h"
#include "csvdatafilereader.h"


Model::Model(QObject* parent) : QObject(parent)
{

}

int Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename);
    const DataFileReadInfo info = dataFileReader->readData();
    delete dataFileReader;

    if (!info.ok)
    {
        emit failedToLoadData(filename);
        return -1;
    }
    else
    {
        m_trips = info.trips;
        m_stations = info.stations;
        emit dataLoaded(m_trips, m_stations);
        return m_trips.size();
    }
}
