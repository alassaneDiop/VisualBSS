#include "csvdatafilereader.h"

#include <QFile>
#include <QVector>
#include <QMap>
#include <QtConcurrent>

#include "trip.h"
#include "station.h"


CsvDataFileReader::CsvDataFileReader(const DataFileParams& params) :
    DataFileReader(params)
{

}

CsvDataFileReader::~CsvDataFileReader()
{

}

DataFileReadInfo CsvDataFileReader::readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const
{
    DataFileReadInfo info;

    // try to open a readonly text file
    QFile file(params().filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        info.ok = false;
        info.errorString = file.errorString();
        return info;
    }

    // sugar syntactic reading and splitting
    QStringList lines = QString(file.readAll()).split('\n');
    if (!lines.isEmpty())
        lines.removeFirst();

    file.close();

    // reserve space
    trips.reserve(lines.size());

    // parsing is parallelized so we need locks to synchronize data access
    QMutex tripsLock, stationsMapLock;

    // the functor that will be run in parallel
    const auto runFunction = [this, &trips, &tripsLock, &stations, &stationsMapLock](QString& line)
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

        const auto insertStation = [&stations, &stationsMapLock](const QString& name, const QString& latitudeStr, const QString& longitudeStr)
        {
            Station s;

            stationsMapLock.lock();
            const auto it = stations.constFind(name);
            const bool found = (it != stations.constEnd());
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
                s.id = stations.size();
                stations.insert(name, s);
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
        t.startDateTime = QDateTime::fromString(startDateTimeStr, params().dateFormat);
        t.endDateTime = QDateTime::fromString(endDateTimeStr, params().dateFormat);

        tripsLock.lock();
        t.id = trips.size();
        trips.append(t);
        tripsLock.unlock();

        if (t.isCyclic)
        {
            stationsMapLock.lock();
            stations[startName].appendCycle(t);
            stationsMapLock.unlock();
        }
        else
        {
            stationsMapLock.lock();
            stations[startName].appendDeparture(t);
            stations[endName].appendArrival(t);
            stationsMapLock.unlock();
        }
    };

    // parses all lines from file in parallel (Qt does all the work for us and chooses an appropriate thread count)
    QtConcurrent::blockingMap(lines, runFunction);

    // some trips may be invalids so we didn't insert them into the vector
    trips.squeeze();

    // to optimize memory usage (because QVector allocates more than needed)
    const auto squeeze = [](Station& s)
    {
        s.arrivalsId.squeeze();
        s.cyclesId.squeeze();
        s.departuresId.squeeze();
    };

    // squeeze trips from station in parallel
    QtConcurrent::blockingMap(stations, squeeze);

    return info;
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
