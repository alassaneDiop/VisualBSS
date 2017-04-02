/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

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
