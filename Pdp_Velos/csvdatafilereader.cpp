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

bool CsvDataFileReader::readData(QVector<Trip>& tripsData, QVector<Station>& stationsData) const
{
    QFile file(DataFileReader::getFilename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines = QString(file.readAll()).split('\n');
    if (!lines.isEmpty())
        lines.removeFirst();

    file.close();

    QMap<QString, Station> stationsDataMap;
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
                Station s;
                s.name = name;
                s.id = stationsDataMap.size() ;
                s.latitude = latitudeStr.toDouble();
                s.longitude = longitudeStr.toDouble();
                stationsDataMap.insert(name, s);
                return s.id;
            }
        };

        Trip t;
        t.id = tripsData.size();
        t.startDateTime = QDateTime::fromString(startDateTimeStr, dateTimeFormat);;
        t.endDateTime =  QDateTime::fromString(endDateTimeStr, dateTimeFormat);
        t.startStationId =  f(startName, startLatitudeStr, startLongitudeStr);
        t.endStationId = f(endName, endLatitudeStr, endLongitudeStr);
        t.duration = QTime().addMSecs(t.startDateTime.msecsTo(t.endDateTime));
        t.isCyclic = (startName == endName);
        if (t.isCyclic)
            stationsDataMap[startName].appendCycle(t);
        else
        {
            stationsDataMap[startName].appendDeparture(t);
            stationsDataMap[endName].appendArrival(t);
        }

        tripsData.append(t);

        // TODO : a retirer
//        if (tripsData.size() >= 1000)
//            break;
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
