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

#include "data.h"
#include "datafilereader.h"

Data::Data(QObject* parent) :
    QObject(parent)
{

}

QVector<Station> Data::stations(const QVector<int>& ids) const
{
    QVector<Station> stations;
    stations.reserve(ids.size());
    for (const int id : ids)
        stations.append(station(id));

    return stations;
}

QVector<Trip> Data::trips(const QVector<int>& ids) const
{
    QVector<Trip> trips;
    trips.reserve(ids.size());
    for (const int id : ids)
        trips.append(trip(id));

    return trips;
}

bool Data::load(const QString& filename, const bool& parallel)
{
    QHash<QString, Station> stationsHash;
    stationsHash.reserve(m_stations.size());
    for (const Station s : m_stations)
        stationsHash.insert(s.name, s);

    const DataFileReader dataFileReader(filename);
    DataFileReadInfo info;
    if (!parallel)
        info = dataFileReader.readData(stationsHash, m_trips);
    else
    {
        QReadWriteLock stationsLock, tripsLock;
        info = dataFileReader.parallelReadData(stationsHash, m_trips, stationsLock, tripsLock);
    }

    if (!info.ok)
    {
        emit failedToLoadData(filename, info.errorString);
        return false;
    }
    else
    {
        m_stations = stationsHash.values().toVector();
        m_hasData = !(m_stations.isEmpty() && m_trips.isEmpty());
        emit dataLoaded(m_trips, m_stations);
        return true;
    }
}

bool Data::clear()
{
    if (!hasData())
    {
        emit failedToClearData();
        return false;
    }
    else
    {
        m_trips.clear();
        m_trips.squeeze();

        m_stations.clear();
        m_stations.squeeze();

        m_hasData = false;
        emit dataCleared();
        return true;
    }
}
