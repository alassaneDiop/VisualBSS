#include "csvdatafilereader.h"

#include <QFile>
#include <QVector>
#include <QMap>
#include <QtConcurrent>

#include "trip.h"
#include "station.h"
#include "config.h"

CsvDataFileReader::CsvDataFileReader(const DataFileParams& params) :
    AbstractDataFileReader(params)
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
    trips.reserve(lines.size());

    for (QString line : lines)
    {
        const QStringList fields = line.remove('"').split(',');
        if (fields.size() < 11)
            continue;

        const QString& startName = fields.at(4);
        const QString& endName = fields.at(8);
        if (startName.isEmpty() || endName.isEmpty())
            continue;

        const QString& durationStr = fields.at(0);
        const QString& startDateTimeStr = fields.at(1);
        const QString& endDateTimeStr = fields.at(2);
        const QString& startLatitudeStr = fields.at(5);
        const QString& startLongitudeStr = fields.at(6);
        const QString& endLatitudeStr = fields.at(9);
        const QString& endLongitudeStr = fields.at(10);

        const auto insertStation = [&stations](const QString& name, const QString& latitudeStr, const QString& longitudeStr)
        {
            Station s;

            const auto it = stations.constFind(name);
            const bool found = (it != stations.constEnd());
            if (found)
                s = it.value();
            else
            {
                s.name = name;
                s.latitude = latitudeStr.toDouble();
                s.longitude = longitudeStr.toDouble();
                s.id = stations.size();
                stations.insert(name, s);
            }
            return s;
        };

        const Station& startStation = insertStation(startName, startLatitudeStr, startLongitudeStr);
        const Station& endStation = insertStation(endName, endLatitudeStr, endLongitudeStr);

        Trip t;
        t.startStationId = startStation.id;
        t.endStationId = endStation.id;
        t.isCyclic = (startName == endName);
        t.duration = durationStr.toInt();
        t.direction = startStation.direction(endStation);
        t.startDateTime = QDateTime::fromString(startDateTimeStr, params().dateFormat);
        t.endDateTime = QDateTime::fromString(endDateTimeStr, params().dateFormat);

        // "For cyclic trips, we estimated the distance by multiplying the duration by the average speed of 2.7m/s."
        if (t.isCyclic)
            t.distance = bss::AVG_SPEED * t.duration;
        else
            t.distance = startStation.distance(endStation);

        t.id = trips.size();
        trips.append(t);

        if (t.isCyclic)
            stations[startName].appendCycle(t);
        else
        {
            stations[startName].appendDeparture(t);
            stations[endName].appendArrival(t);
        }
    }

    // some trips may be invalids so we didn't insert them into the vector
    if (trips.count() < lines.count())
        trips.squeeze();

    // to optimize memory usage (because QVector allocates more than needed)
    const auto squeeze = [](Station& s)
    {
        s.arrivalsIds.squeeze();
        s.cyclesIds.squeeze();
        s.departuresIds.squeeze();
    };

    // squeeze trips from station in parallel
    QtConcurrent::blockingMap(stations, squeeze);

    return info;
}

DataFileReadInfo CsvDataFileReader::parallelReadData(QHash<QString, Station>& stations,
                                                     QVector<Trip>& trips,
                                                     QReadWriteLock& stationsLock,
                                                     QReadWriteLock& tripsLock) const
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
    trips.reserve(lines.size());

    // the functor that will be run in parallel
    const auto runFunction = [this, &trips, &tripsLock, &stations, &stationsLock](QString& line)
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

        const auto insertStation = [&stations, &stationsLock](const QString& name, const QString& latitudeStr, const QString& longitudeStr)
        {
            Station s;

            stationsLock.lockForRead();
            const auto it = stations.constFind(name);
            const bool found = (it != stations.constEnd());
            if (found)
            {
                s = it.value();
                stationsLock.unlock();
            }
            else
            {
                stationsLock.unlock();

                s.name = name;
                s.latitude = latitudeStr.toDouble();
                s.longitude = longitudeStr.toDouble();

                stationsLock.lockForWrite();
                s.id = stations.size();
                stations.insert(name, s);
                stationsLock.unlock();
            }
            return s;
        };

        const Station& startStation = insertStation(startName, startLatitudeStr, startLongitudeStr);
        const Station& endStation = insertStation(endName, endLatitudeStr, endLongitudeStr);

        Trip t;
        t.startStationId = startStation.id;
        t.endStationId = endStation.id;
        t.isCyclic = (startName == endName);
        t.duration = durationStr.toInt();
        t.direction = startStation.direction(endStation);
        t.startDateTime = QDateTime::fromString(startDateTimeStr, params().dateFormat);
        t.endDateTime = QDateTime::fromString(endDateTimeStr, params().dateFormat);

        // "For cyclic trips, we estimated the distance by multiplying the duration by the average speed of 2.7m/s."
        if (t.isCyclic)
            t.distance = bss::AVG_SPEED * t.duration;
        else
            t.distance = startStation.distance(endStation);

        tripsLock.lockForWrite();
        t.id = trips.size();
        trips.append(t);
        tripsLock.unlock();

        if (t.isCyclic)
        {
            stationsLock.lockForWrite();
            stations[startName].appendCycle(t);
            stationsLock.unlock();
        }
        else
        {
            stationsLock.lockForWrite();
            stations[startName].appendDeparture(t);
            stations[endName].appendArrival(t);
            stationsLock.unlock();
        }
    };

    // parses all lines from file in parallel (Qt does all the work for us and chooses an appropriate thread count)
    QtConcurrent::blockingMap(lines, runFunction);

    // some trips may be invalids so we didn't insert them into the vector
    if (trips.count() < lines.count())
        trips.squeeze();

    // to optimize memory usage (because QVector allocates more than needed)
    const auto squeeze = [](Station& s)
    {
        s.arrivalsIds.squeeze();
        s.cyclesIds.squeeze();
        s.departuresIds.squeeze();
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
