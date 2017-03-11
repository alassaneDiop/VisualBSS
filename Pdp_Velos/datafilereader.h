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
    DataFileReader(const QString& filename, const Qt::DateFormat& dateFormat);
    virtual ~DataFileReader();

    inline QString filename() const { return m_filename; }
    inline Qt::DateFormat dateFormat() const { return m_dateFormat; }

    virtual bool readData(QVector<Trip>& tripsData, QVector<Station>& stationsData) const = 0;

private:  
    const QString m_filename; 
    const Qt::DateFormat m_dateFormat;
};

#endif // DATAFILEREADER_H
