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
    virtual QSet<const Station*> readData(bool* ok = nullptr) const override;
};

#endif // XMLDATAFILEREADER_H
