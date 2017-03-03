#include "csvdatafilereader.h"
#include "trip.h"
#include "station.h"

#include <QFile>
#include <QSet>
#include <QVector>

CsvDataFileReader::CsvDataFileReader(const QString& filename) :
    DataFileReader(filename)
{

}

QSet<const Station*> CsvDataFileReader::readData(bool* ok) const
{
    QFile file(DataFileReader::getFilename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        *ok = false;
        return QSet<const Station*>();
    }
    else
    {
        QStringList lines = QString(file.readAll()).split('\n');
        if (!lines.isEmpty())
            lines.removeFirst();

        file.close();
        *ok = true;

        /// pour accelerer les tests on seulement i lignes du fichier
        // TODO: enlever le i = 200;
        int i = 200;

        QVector<Station> stations;
// debut de section parallele
        for (QString line : lines)
        {
            //const Data data = parseCsvData(line);
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

                const Station startStation = Station(startStationName, startLatitudeStr.toDouble(), startLongitudeStr.toDouble());
                const Station endStation = Station(endStationName, endLatitudeStr.toDouble(), endLongitudeStr.toDouble());

                if (!stations.contains(startStation) && startStation.isValid())
                    stations.append(startStation);

                if (!stations.contains(endStation) && endStation.isValid())
                    stations.append(endStation);

                const QString dateTimeFormat = "YYYY-MM-dd HH-mm-ss";
                const QDateTime startDateTime = QDateTime::fromString(startDateTimeStr, dateTimeFormat);
                const QDateTime endDateTime = QDateTime::fromString(endDateTimeStr, dateTimeFormat);
                Station* const startStationPtr = &stations[stations.indexOf(startStation)];
                Station* const endStationPtr = &stations[stations.indexOf(endStation)];
                const Trip* trip = &Trip(startStationPtr, endStationPtr, startDateTime, endDateTime);

                startStationPtr->insertOutgoingTrip(trip);
                startStationPtr->insertIncomingTrip(trip);
                endStationPtr->insertIncomingTrip(trip);
                endStationPtr->insertOutgoingTrip(trip);
            }
// debut de section critique                 
// fin de section critique

            i--;
            if (i == 0)
                break;
        }
// fin de section parallele

        QSet<const Station*> result;
        for (const Station station : stations)
            result.insert(&station);

        return result;
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
