#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include "station.h"
#include <QString>

namespace bss {
class DataFileReader;
}

class DataFileReader
{
public:
    DataFileReader(const QString& filename);
    virtual ~DataFileReader();
    inline QString getFilename() const { return m_filename; }
    virtual QSet<const Station*> readData(bool* ok = nullptr) const = 0;

private:  
    const QString m_filename; 
};

#endif // DATAFILEREADER_H
