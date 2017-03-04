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
    virtual bool readData(QVector<const Trip*>& trips, QVector<const Station*>& stations) const override;
};

#endif // XMLDATAFILEREADER_H
