#include "datafilereader.h"

#include <QFile>
#include <QMutex>
#include <QtConcurrent>

#include "csvdatafilereader.h"
#include "xmldatafilereader.h"


DataFileReader::DataFileReader(const DataFileParams& params) :
    AbstractDataFileReader(params)
{

}

DataFileReader::DataFileReader(const QString& filename, const Qt::DateFormat& dateFormat) :
    AbstractDataFileReader(filename, dateFormat)
{

}

DataFileReader::~DataFileReader()
{

}

DataFileReadInfo DataFileReader::readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const
{   
    const QString suffix = QFileInfo(params().filename).completeSuffix();
    if (suffix == "csv")
    {
        const CsvDataFileReader reader = CsvDataFileReader(params());
        return reader.readData(stations, trips);
    }
    if (suffix == "xml")
    {
        const XmlDataFileReader reader = XmlDataFileReader(params());
        return reader.readData(stations, trips);
    }
    else
    {
        DataFileReadInfo info;
        info.ok = false;
        info.errorString = "Error : unsupported format";
        return info;
    }
}

DataFileReadInfo DataFileReader::parallelReadData(QHash<QString, Station>& stations,
                                                  QVector<Trip>& trips,
                                                  QReadWriteLock& stationsLock,
                                                  QReadWriteLock& tripsLock) const
{
    const QString suffix = QFileInfo(params().filename).completeSuffix();
    if (suffix == "csv")
    {
        const CsvDataFileReader reader = CsvDataFileReader(params());
        return reader.parallelReadData(stations, trips, stationsLock, tripsLock);
    }
    if (suffix == "xml")
    {
        const XmlDataFileReader reader = XmlDataFileReader(params());
        return reader.parallelReadData(stations, trips, stationsLock, tripsLock);
    }
    else
    {
        DataFileReadInfo info;
        info.ok = false;
        info.errorString = "Error : unsupported format";
        return info;
    }
}

/*
 * For CitiBike:
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
