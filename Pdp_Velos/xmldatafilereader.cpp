#include "xmldatafilereader.h"

#include <QFile>
#include <QXmlStreamReader>

XmlDataFileReader::XmlDataFileReader(const QString& filename) :
    DataFileReader(filename)
{

}

int XmlDataFileReader::readData(QSet<Station>& stations, QSet<Trip>& trips) const
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
    return false;
}
