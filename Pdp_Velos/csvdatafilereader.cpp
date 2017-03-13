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

    // clear trips vector (if it's not) and reserve space
    trips.clear();
    trips.reserve(lines.size());

    // parsing is parallelized so we need locks to synchronize data access
    QMutex tripsLock, stationsMapLock;

    // at this point, each station is identified by its name
    QHash<const QString, Station> stationsMap;

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

            stationsMapLock.lock();
            const auto it = stationsMap.constFind(name);
            const bool found = (it != stationsMap.constEnd());
            if (found)
            {
                s = it.value();
                stationsMapLock.unlock();
            }
            else
            {
                stationsMapLock.unlock();

                s.name = name;
                s.latitude = latitudeStr.toDouble();
                s.longitude = longitudeStr.toDouble();

                stationsMapLock.lock();
                s.id = stationsMap.size();
                stationsMap.insert(name, s);
                stationsMapLock.unlock();
            }
            return s;
        };

        const Station& startStation = insertStation(startName, startLatitudeStr, startLongitudeStr);
        const Station& endStation = insertStation(endName, endLatitudeStr, endLongitudeStr);

        Trip t;
        t.startStationId = startStation.id;
        t.endStationId = endStation.id;
        t.isCyclic = (startName == endName);
        t.duration = durationStr.toDouble();
        t.distance = startStation.distance(endStation);
        t.direction = startStation.direction(endStation);
        t.startDateTime = QDateTime::fromString(startDateTimeStr, dateFormat());
        t.endDateTime = QDateTime::fromString(endDateTimeStr, dateFormat());

        tripsLock.lock();
        t.id = trips.size();
        trips.append(t);
        tripsLock.unlock();

        if (t.isCyclic)
        {
            stationsMapLock.lock();
            stationsMap[startName].appendCycle(t);
            stationsMapLock.unlock();
        }
        else
        {
            stationsMapLock.lock();
            stationsMap[startName].appendDeparture(t);
            stationsMap[endName].appendArrival(t);
            stationsMapLock.unlock();
        }
    };

    // parses all lines from file in parallel (Qt does all the work for us and chooses an appropriate thread count)
    QtConcurrent::blockingMap(lines, runFunction);

    // some trips may be invalids so we didn't insert them into the vector
    trips.squeeze();

    // transmits stations from map to vector
    stations = QVector<Station>::fromList(stationsMap.values());

    // to optimize memory usage (because QVector allocates more than needed)
    const auto squeeze = [](Station& s)
    {
        s.arrivalsId.squeeze();
        s.cyclesId.squeeze();
        s.departuresId.squeeze();
    };

    // squeeze trips from station in parallel
    QtConcurrent::blockingMap(stations, squeeze);

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
