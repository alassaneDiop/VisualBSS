#include "model.h"
#include "datafilereader.h"
#include "csvdatafilereader.h"

#include "trip.h"
#include "station.h"

Model::Model(QObject* parent) :
    QObject(parent)
{

}

Model::~Model()
{
    for (const Trip* t : m_trips)
        delete t;

    for (const Station* s : m_stations)
        delete s;
}

int Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename);
    const bool ok = dataFileReader->readData(m_trips, m_stations);
    delete dataFileReader;

    if (!ok)
    {
        emit failedToLoadData(filename);
        return -1;
    }
    else
    {
        emit dataLoaded(m_trips, m_stations);
        return m_trips.size();
    }
}
