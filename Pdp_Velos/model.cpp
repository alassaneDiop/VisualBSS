#include "model.h"
#include "datafilereader.h"

#include "csvdatafilereader.h"

Model::Model(QObject* parent) : QObject(parent)
{

}

int Model::loadData(const QString& filename)
{
    const DataFileReader* dataFileReader = new CsvDataFileReader(filename);
    const int result = dataFileReader->readData(m_trips);
    delete dataFileReader;

    if (result >= 0)
    {
        m_stations = getStationsFromTrips(getTrips());
        emit dataLoaded(getStations(), getTrips());
    }

    return result;
}

QSet<Station> Model::getStationsFromTrip(const Trip& trip)
{
    QSet<Station> stations;
    stations.insert(trip.getStartStation());
    stations.insert(trip.getEndStation());
    return stations;
}

QSet<Station> Model::getStationsFromTrips(const QList<Trip>& trips)
{
    QSet<Station> stations;
    for (const Trip& trip : trips)
        stations += getStationsFromTrip(trip);

    return stations;
}
