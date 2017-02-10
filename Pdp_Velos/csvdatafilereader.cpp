#include "csvdatafilereader.h"

#include "trip.h"
#include "station.h"

#include <QFile>
#include <QSet>
#include <QTime>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

CsvDataFileReader::CsvDataFileReader(const QString& filename) :
    DataFileReader(filename)
{

}

bool CsvDataFileReader::readData(QSet<Trip>& trips) const
{
    QFile file(DataFileReader::getFilename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    else
    {
        trips.clear();

        QStringList lines = QString(file.readAll()).split('\n');
        file.close();

        if (!lines.isEmpty())
        {
            lines.removeFirst();

            QFuture<Trip> future = QtConcurrent::mapped(lines, parseTrip);
            future.waitForFinished();
            for (const Trip& trip : future)
                if (trip.isValid())
                    trips.insert(trip);
        }

        return true;
    }
}

QDate CsvDataFileReader::parseDate(const QString& dateString)
{
    const QStringList dateStrings = dateString.split('-');
    if (dateStrings.size() != 3)
        return QDate();
    else
    {
        const int y = QString(dateStrings[0]).toInt();
        const int m = QString(dateStrings[1]).toInt();
        const int d = QString(dateStrings[2]).toInt();
        return QDate(y, m, d);
    }
}

QTime CsvDataFileReader::parseTime(const QString& timeString)
{
    const QStringList timeStrings = timeString.split(':');
    if (timeStrings.size() != 3)
        return QTime();
    else
    {
        const int h = QString(timeStrings[0]).toInt();
        const int m = QString(timeStrings[1]).toInt();
        const int s = QString(timeStrings[2]).toInt();
        return QTime(h, m, s);
    }
}

QDateTime CsvDataFileReader::parseDateTime(const QString& dateTimeString)
{
    const QStringList dateTimeStrings = dateTimeString.split(' ');
    if (dateTimeStrings.size() != 2)
        return QDateTime();
    else
    {
        const QDate date = parseDate(dateTimeStrings[0]);
        const QTime time = parseTime(dateTimeStrings[1]);
        return QDateTime(date, time);
    }
}

Station CsvDataFileReader::parseStation(const QString& stationName, const QString& latitudeString, const QString& longitudeString)
{
    const qreal latitude = latitudeString.toDouble();
    const qreal longitude = longitudeString.toDouble();
    return Station(stationName, latitude, longitude);
}

Trip CsvDataFileReader::parseTrip(QString line)
{
    const QStringList fields = line.remove('"').split(',');
    if (fields.size() < 11)
        return Trip();
    else
    {
        const QString durationString = fields[0];
        const QString startDateTimeString = fields[1];
        const QString endDateTimeString = fields[2];
        const QString startStationName = fields[4];
        const QString startStationLatitudeString = fields[5];
        const QString startStationLongitudeString = fields[6];
        const QString endStationName = fields[8];
        const QString endStationLatitudeString = fields[9];
        const QString endStationLongitudeString = fields[10];

        const Station startStation = parseStation(startStationName, startStationLatitudeString, startStationLongitudeString);
        const Station endStation = parseStation(endStationName, endStationLatitudeString, endStationLongitudeString);
        const QDateTime startDateTime = parseDateTime(startDateTimeString);
        const QDateTime endDateTime = parseDateTime(endDateTimeString);
        const QTime duration = QTime(0, 0, 0).addSecs(durationString.toInt());
        return Trip(startStation, endStation, startDateTime, endDateTime, duration);
    }
}

/*
"tripduration",
"starttime",
"stoptime",
"start station id",
"start station name",
"start station latitude",
"start station longitude",
"end station id",
"end station name",
"end station latitude",
"end station longitude",*/
