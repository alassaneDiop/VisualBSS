#include "model.h"
#include "datafilereader.h"

#include "csvdatafilereader.h"

Model::Model(QObject* parent) : QObject(parent)
{

}

bool Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename);
    const bool result = dataFileReader->readData(m_trips);
    delete dataFileReader;
    return result;
}
