#include "model.h"
#include "datafilereader.h"

#include "csvdatafilereader.h"

Model::Model(QObject* parent) : QObject(parent)
{

}

int Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename);
    const int result = dataFileReader->readData(m_stations, m_trips);
    delete dataFileReader;
    return result;
}
