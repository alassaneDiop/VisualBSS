#include "model.h"
#include "datafilereader.h"
#include "csvdatafilereader.h"


Model::Model(QObject* parent) : QObject(parent)
{

}

int Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = &CsvDataFileReader(filename);
    const DataFileReadInfo info = dataFileReader->readData();
    if (!info.ok)
    {
        emit failedToLoadData(filename);
        return -1;
    }
    else
    {
        m_stations = info.stations;
        emit dataLoaded(getStations());
        return info.tripsCount;
    }
}
