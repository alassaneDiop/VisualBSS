#include "csvdatafilereader.h"
#include "trip.h"
#include "station.h"

#include <QFile>
#include <QTime>
#include <QVector>
#include <QMap>

CsvDataFileReader::CsvDataFileReader(const QString& filename) :
    DataFileReader(filename)
{

}

CsvDataFileReader::~CsvDataFileReader()
{

}

bool CsvDataFileReader::readData(QVector<TripData>& tripsData, QVector<StationData>& stationsData) const
{
    QFile file(DataFileReader::getFilename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines = QString(file.readAll()).split('\n');
    if (!lines.isEmpty())
        lines.removeFirst();

    file.close();

    QMap<QString, StationData> stationsDataMap;
    tripsData.clear();
    tripsData.reserve(lines.size());

    for (QString line : lines)
    {
        const QStringList fields = line.remove('"').split(',');
        if (fields.size() < 11)
            continue;

        const QString startDateTimeStr = fields.at(1);
        const QString endDateTimeStr = fields.at(2);
        const QString startName = fields.at(4);
        const QString startLatitudeStr = fields.at(5);
        const QString startLongitudeStr = fields.at(6);
        const QString endName = fields.at(8);
        const QString endLatitudeStr = fields.at(9);
        const QString endLongitudeStr = fields.at(10);

        if (startName.isEmpty() || endName.isEmpty())
            continue;

        auto f = [&stationsDataMap](const QString& name, const QString& latitudeStr, const QString& longitudeStr)
        {
            if (stationsDataMap.contains(name))
                return stationsDataMap.value(name).id;
            else
            {
                const int id = stationsDataMap.size();
                const qreal lat = latitudeStr.toDouble();
                const qreal lon = longitudeStr.toDouble();
                stationsDataMap.insert(name, StationData(id, name, lat, lon));
                return id;
            }
        };

        const int startId = f(startName, startLatitudeStr, startLongitudeStr);
        const int endId = f(endName, endLatitudeStr, endLongitudeStr);
        const int tripId = tripsData.size();

        const QDateTime startDateTime = QDateTime::fromString(startDateTimeStr, dateTimeFormat);
        const QDateTime endDateTime = QDateTime::fromString(endDateTimeStr, dateTimeFormat);

        const bool isCyclic = (startName == endName);
        TripData tripData = TripData(tripId, startId, endId, isCyclic, startDateTime, endDateTime);
        tripData.durationMsec = startDateTime.msecsTo(endDateTime);
        if (isCyclic)
            stationsDataMap[startName].appendCycle(tripId);
        else
        {
            stationsDataMap[startName].appendDeparture(tripId);
            stationsDataMap[endName].appendArrival(tripId);
        }

        tripsData.append(tripData);

        // TODO : a retirer
        if (tripsData.size() >= 20)
            break;
    }

    stationsData = stationsDataMap.values().toVector();
    tripsData.squeeze();
    return true;
}

/*
 * Pour CitiBike:
 * "tripduration",
 * "starttime",
 * "stoptime",
 * "start station id",
 * "start station name",
 * "start station latitude",
 * "start station longitude",
 * "end station id",
 * "end station name",
 * "end station latitude",
 * "end station longitude",
*/
