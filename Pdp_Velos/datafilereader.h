#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <QString>
#include "trip.h"

namespace bss {
class DataFileReader;
}

class DataFileReader
{
public:
    DataFileReader(const QString& filename);
    inline QString getFilename() const { return m_filename; }
    virtual int readData(QSet<Station>& stations, QSet<Trip>& trips) const = 0;

private:
    const QString m_filename;
};

#endif // DATAFILEREADER_H
