#ifndef XMLDATAFILEREADER_H
#define XMLDATAFILEREADER_H

#include "abstractdatafilereader.h"

#include <QString>


namespace bss {
class XmlDataFileReader;
}

class XmlDataFileReader : public AbstractDataFileReader
{
public:
    XmlDataFileReader(const DataFileParams& params);
    virtual ~XmlDataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const override;
};

#endif // XMLDATAFILEREADER_H
