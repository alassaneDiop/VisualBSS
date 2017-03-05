#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include "trip.h"
#include "station.h"
#include <QString>
#include <QVector>

namespace bss {
class DataFileReader;
}

class DataFileReader
{
public:
    DataFileReader(const QString& filename);
    virtual ~DataFileReader();
    inline QString getFilename() const { return m_filename; }
    virtual bool readData(QVector<TripData>& tripsData, QVector<StationData>& stationsData) const = 0;

private:  
    const QString m_filename; 
};

#endif // DATAFILEREADER_H
