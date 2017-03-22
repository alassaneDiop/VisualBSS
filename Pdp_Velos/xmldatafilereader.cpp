#include "xmldatafilereader.h"

#include <QFile>
#include <QXmlStreamReader>


XmlDataFileReader::XmlDataFileReader(const DataFileParams& params) :
    AbstractDataFileReader(params)
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
