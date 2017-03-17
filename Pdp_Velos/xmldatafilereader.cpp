#include "xmldatafilereader.h"

#include <QFile>
#include <QXmlStreamReader>


XmlDataFileReader::XmlDataFileReader(const DataFileParams& params) :
    DataFileReader(params)
{

}

XmlDataFileReader::~XmlDataFileReader()
{

}

DataFileReadInfo XmlDataFileReader::readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const
{   
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
