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

#include "xmldatafilereader.h"

#include <QFile>
#include <QXmlStreamReader>


XmlDataFileReader::XmlDataFileReader(const DataFileParams& params) :
    AbstractDataFileReader(params)
{

}

XmlDataFileReader::XmlDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat) :
    AbstractDataFileReader(filename, dateFormat)
{

}

XmlDataFileReader::~XmlDataFileReader()
{

}

DataFileReadInfo XmlDataFileReader::readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const
{
    Q_UNUSED(stations);
    Q_UNUSED(trips);

    /*QFile file(DataFileReader::getFilename());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    else
    {
        QXmlStreamReader streamReader = QXmlStreamReader(file);

        //Parse the XML until we reach end of it
        while(!streamReader.atEnd() && !streamReader.hasError())
        {
            // Read next element
            QXmlStreamReader::TokenType token = streamReader.readNext();

            if (token.isStartDocument())
                continue;

            if (token.isStartElement())
            {
                if (streamReader.name() == "id")
                    qDebug() << streamReader.readElementText();
            }
        }

        file.close();
        return true;
    }*/
    return DataFileReadInfo();
}

DataFileReadInfo XmlDataFileReader::parallelReadData(QHash<QString, Station>& stations,
                                                     QVector<Trip>& trips,
                                                     QReadWriteLock& stationsLock,
                                                     QReadWriteLock& tripsLock) const
{
    Q_UNUSED(stations);
    Q_UNUSED(trips);
    Q_UNUSED(stationsLock);
    Q_UNUSED(tripsLock);
    return DataFileReadInfo();
}
