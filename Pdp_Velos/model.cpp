#include "model.h"
#include "datafilereader.h"

#include "csvdatafilereader.h"

Model::Model(QObject* parent) :
    QObject(parent)
{

}

bool Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = &CsvDataFileReader(filename);
    bool ok;
    m_stations = dataFileReader->readData(&ok);
    if (!ok)
        return false;
    else
    {
        emit dataLoaded(getStations());
        return true;
    }
}
