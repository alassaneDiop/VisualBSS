#ifndef XMLDATAFILEREADER_H
#define XMLDATAFILEREADER_H

#include "datafilereader.h"

#include "trip.h"
#include <QString>

namespace bss {
class XmlDataFileReader;
}

class XmlDataFileReader : public DataFileReader
{
public:
    XmlDataFileReader(const QString& m_filename);
    virtual bool readData(QSet<Trip>& data) const override;
};

#endif // XMLDATAFILEREADER_H
