#include "csvdatafilereader.h"
#include "trip.h"
#include "station.h"

#include <QFile>
#include <QTime>
#include <QVector>

CsvDataFileReader::CsvDataFileReader(const QString& filename) :
    DataFileReader(filename)
{

}

CsvDataFileReader::~CsvDataFileReader()
{

}

bool CsvDataFileReader::readData(QVector<const Trip*>& trips, QVector<const Station*>& stations) const
{
    QFile file(DataFileReader::getFilename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    else
    {
        QStringList lines = QString(file.readAll()).split('\n');
        if (!lines.isEmpty())
            lines.removeFirst();

        file.close();

        stations.clear();
        trips.clear();
        trips.reserve(lines.size());

// debut de section parallele
        for (QString line : lines)
        {
            const QStringList fields = line.remove('"').split(',');
            if (fields.size() >= 11)
            {
                const QString startDateTimeStr = fields.at(1);
                const QString endDateTimeStr = fields.at(2);
                const QString startStationName = fields.at(4);
                const QString startLatitudeStr = fields.at(5);
                const QString startLongitudeStr = fields.at(6);
                const QString endStationName = fields.at(8);
                const QString endLatitudeStr = fields.at(9);
                const QString endLongitudeStr = fields.at(10);

                Station* const startStation = new Station(startStationName, startLatitudeStr.toDouble(), startLongitudeStr.toDouble());
                Station* const endStation = new Station(endStationName, endLatitudeStr.toDouble(), endLongitudeStr.toDouble());

// debut de section critique
                bool containsStartStation = false;
                QVector<const Station*>::iterator s = stations.begin();
                for (s; s < stations.end() && !containsStartStation; ++s)
                    containsStartStation = (*startStation == **s);

                if (!containsStartStation)
                    stations.append(startStation);

                if (*startStation != *endStation)
                {
                    bool containsEndStation = false;
                    QVector<const Station*>::iterator s = stations.begin();
                    for (s; s < stations.end() && !containsEndStation; ++s)
                        containsEndStation = (*endStation == **s);

                    if (!containsEndStation)
                        stations.append(endStation);
                }

                // TODO : retirer codage dans le dur du format du temps
                const QString dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
                const QDateTime startDateTime = QDateTime::fromString(startDateTimeStr, dateTimeFormat);
                const QDateTime endDateTime = QDateTime::fromString(endDateTimeStr, dateTimeFormat);
                const Trip* trip = new Trip(startStation, endStation, startDateTime, endDateTime);
                if (trip->isValid())
                    trips.append(trip);

                // TODO : retirer limite de nombre de trajets
                if (trips.size() >= 1000)
                    break;
// fin de section critique
            }              
        }
// fin de section parallele

        trips.squeeze();
        return true;
    }
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
