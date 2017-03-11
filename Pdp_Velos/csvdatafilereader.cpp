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
    QFile file(DataFileReader::filename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines = QString(file.readAll()).split('\n');
    if (!lines.isEmpty())
        lines.removeFirst();

    file.close();

    QHash<const QString&, Station> stationsMap;

    trips.clear();
    trips.reserve(lines.size());

    /*for (QString line : lines)
    {
        const QStringList fields = line.remove('"').split(',');
        if (fields.size() < 11)
            continue;

        const QString& startName = fields.at(4);
        const QString& endName = fields.at(8);

        if (startName.isEmpty() || endName.isEmpty())
            continue;

        const QString& startDateTimeStr = fields.at(1);
        const QString& endDateTimeStr = fields.at(2);
        const QString& startLatitudeStr = fields.at(5);
        const QString& startLongitudeStr = fields.at(6);
        const QString& endLatitudeStr = fields.at(9);
        const QString& endLongitudeStr = fields.at(10);

        const auto insertStation = [&stationsMap](const QString& name, const QString& latitudeStr, const QString& longitudeStr)
        {
            const auto it = stationsMap.find(name);
            if (it != stationsMap.end())
                return it.value();
            else
            {
                Station s;
                s.name = name;
                s.id = stationsMap.size() ;
                s.latitude = latitudeStr.toDouble();
                s.longitude = longitudeStr.toDouble();
                stationsMap.insert(name, s);
                return s;
            }
        };

        const Station& startStation = insertStation(startName, startLatitudeStr, startLongitudeStr);
        const Station& endStation = insertStation(endName, endLatitudeStr, endLongitudeStr);

        Trip t;
        t.id = trips.size();

        t.startDateTime = QDateTime::fromString(startDateTimeStr, dateFormat());
        t.endDateTime = QDateTime::fromString(endDateTimeStr, dateFormat());

        t.startStationId = startStation.id;
        t.endStationId = endStation.id;

        t.distance = startStation.distance(endStation);
        t.direction = startStation.direction(endStation);

        t.duration = t.startDateTime.secsTo(t.endDateTime);
        t.isCyclic = (startName == endName);

        if (t.isCyclic)
            stationsMap[startName].appendCycle(t);
        else
        {
            stationsMap[startName].appendDeparture(t);
            stationsMap[endName].appendArrival(t);
        }

        trips.append(t);

        // TODO : a retirer
        //        if (tripsData.size() >= 1000)
        //            break;
    }*/

    QReadWriteLock tripsLock;
    QReadWriteLock stationsMapLock;

    const auto runFunction = [this, &trips, &tripsLock, &stationsMap, &stationsMapLock](QString& line)
    {
        const QStringList fields = line.remove('"').split(',');
        if (fields.size() < 11)
            return;

        const QString& startName = fields.at(4);
        const QString& endName = fields.at(8);
        if (startName.isEmpty() || endName.isEmpty())
            return;

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
        t.duration = t.startDateTime.secsTo(t.endDateTime);
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

    /*QThreadPool pool;
    pool.setExpiryTimeout(-1);

    const int linesCount = lines.size();
    const int threadCount = pool.maxThreadCount();
    QVector<QStringList> lists;
    int pos = 0;
    int len = (linesCount / threadCount);
    for (int i = 0; i < threadCount; ++i)
    {
        lists.append(lines.mid(pos, len));
        pos += len;
        if (i == (threadCount - 1) && ((linesCount % threadCount) != 0))
            len += 1;
    }

    QVector<QFuture<void>> results;
    for (const QStringList l : lists)
        results.append(QtConcurrent::run(runFunction, l));

    for (QFuture<void> r : results)
        r.waitForFinished();*/

    QtConcurrent::blockingMap(lines, runFunction);

    stations = stationsMap.values().toVector();

    // some trips may be invalids so we didn't insert them into the vector
    trips.squeeze();

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
