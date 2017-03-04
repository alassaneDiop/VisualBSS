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
    XmlDataFileReader(const QString& filename);
    virtual ~XmlDataFileReader();
    virtual bool readData(QVector<Trip>& trips, QVector<Station>& stations) const override;
};

#endif // XMLDATAFILEREADER_H
