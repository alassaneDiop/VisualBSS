#include "csvdatafilereader.h"
#include "trip.h"
#include "station.h"

#include <QFile>
#include <QVector>
#include <QMap>
#include <QtConcurrent>

CsvDataFileReader::CsvDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat) :
    DataFileReader(filename, dateFormat)
{

}

CsvDataFileReader::~CsvDataFileReader()
{

}

bool CsvDataFileReader::readData(QVector<Trip>& trips, QVector<Station>& stations) const
{
    // try to open a readonly text file
    QFile file(DataFileReader::filename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // sugar syntactic reading and splitting
    QStringList lines = QString(file.readAll()).split('\n');
    if (!lines.isEmpty())
        lines.removeFirst();

    file.close();

    // parsing is parallelized so we need locks to synchronize data access
    QReadWriteLock tripsLock, stationsMapLock;

    // at this point, each station is identified by its name
    QHash<const QString, Station> stationsMap;

    // clear trips vector (if it's not) and reserve space
    trips.clear();
    trips.reserve(lines.size());

    // the functor that will be run in parallel
    const auto runFunction = [this, &trips, &tripsLock, &stationsMap, &stationsMapLock](QString& line)
    {
        const QStringList fields = line.remove('"').split(',');
        if (fields.size() < 11)
            return;

        const QString& startName = fields.at(4);
        const QString& endName = fields.at(8);
        if (startName.isEmpty() || endName.isEmpty())
            return;

        const QString& durationStr = fields.at(0);
        const QString& startDateTimeStr = fields.at(1);
        const QString& endDateTimeStr = fields.at(2);
        const QString& startLatitudeStr = fields.at(5);
        const QString& startLongitudeStr = fields.at(6);
        const QString& endLatitudeStr = fields.at(9);
        const QString& endLongitudeStr = fields.at(10);

        const auto insertStation = [&stationsMap, &stationsMapLock](const QString& name, const QString& latitudeStr, const QString& longitudeStr)
        {
            Station s;
            stationsMapLock.lockForRead();
            const auto it = stationsMap.constFind(name);
            if (it != stationsMap.constEnd())
            {
                stationsMapLock.unlock();
                s = it.value();
            }
            else
            {
                stationsMapLock.unlock();

                s.name = name;
                s.id = stationsMap.size() ;
                s.latitude = latitudeStr.toDouble();
                s.longitude = longitudeStr.toDouble();

                stationsMapLock.lockForWrite();
                stationsMap.insert(name, s);
                stationsMapLock.unlock();
            }
            return s;
        };

        const Station& startStation = insertStation(startName, startLatitudeStr, startLongitudeStr);
        const Station& endStation = insertStation(endName, endLatitudeStr, endLongitudeStr);

        Trip t;

        tripsLock.lockForRead();
        t.id = trips.size();
        tripsLock.unlock();

        t.startStationId = startStation.id;
        t.endStationId = endStation.id;
        t.startDateTime = QDateTime::fromString(startDateTimeStr, dateFormat());
        t.endDateTime = QDateTime::fromString(endDateTimeStr, dateFormat());
        t.distance = startStation.distance(endStation);
        t.direction = startStation.direction(endStation);
        t.duration = durationStr.toDouble();
        t.isCyclic = (startName == endName);

        stationsMapLock.lockForWrite();
        if (t.isCyclic)
            stationsMap[startName].appendCycle(t);
        else
        {
            stationsMap[startName].appendDeparture(t);
            stationsMap[endName].appendArrival(t);
        }
        stationsMapLock.unlock();

        tripsLock.lockForWrite();
        trips.append(t);
        tripsLock.unlock();
    };

    // parses all lines from file in parallel (Qt does all the work for us and chooses an appropriate threads count)
    QtConcurrent::blockingMap(lines, runFunction);

    // some trips may be invalids so we didn't insert them into the vector
    trips.squeeze();

    // transmits stations from map to vector
    stations = stationsMap.values().toVector();

    // to optimize memory usage (because QVector allocates more than needed)
    for (Station s : stations)
    {
        s.arrivalsId.squeeze();
        s.cyclesId.squeeze();
        s.departuresId.squeeze();
    }

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
