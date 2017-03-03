#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include "station.h"
#include <QString>
#include <QVector>

namespace bss {
class DataFileReader;
struct DataFileReadInfo;
}

struct DataFileReadInfo
{
    const bool ok;
    const QVector<Trip> trips;
    const QVector<Station> stations;

    DataFileReadInfo() : ok(false) { }

    DataFileReadInfo(const bool& ok,
                     const QVector<Trip>& trips,
                     const QVector<Station>& stations) :
        ok(ok), trips(trips), stations(stations) { }
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
