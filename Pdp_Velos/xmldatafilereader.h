#ifndef XMLDATAFILEREADER_H
#define XMLDATAFILEREADER_H

#include "datafilereader.h"

#include <QString>

namespace bss {
class XmlDataFileReader;
}

class XmlDataFileReader : public DataFileReader
{
public:
    XmlDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat);
    virtual ~XmlDataFileReader();
    virtual bool readData(QVector<Trip>& tripsData, QVector<Station>& stationsData) const override;
};

#endif // XMLDATAFILEREADER_H
