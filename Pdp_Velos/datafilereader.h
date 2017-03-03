#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include "station.h"
#include <QString>

namespace bss {
class DataFileReader;
struct DataFileReadInfo;
}

struct DataFileReadInfo
{
    const bool ok;
    const int tripsCount;
    const QSet<const Station*> stations;
    DataFileReadInfo() : ok(false), tripsCount(0) { }
    DataFileReadInfo(const bool& ok, const int& tripsCount, const QSet<const Station*> stations) :
        ok(ok), tripsCount(tripsCount), stations(stations) { }
};

class DataFileReader
{
public:
    DataFileReader(const QString& filename);
    virtual ~DataFileReader();
    inline QString getFilename() const { return m_filename; }
    virtual DataFileReadInfo readData() const = 0;

private:  
    const QString m_filename; 
};

#endif // DATAFILEREADER_H
