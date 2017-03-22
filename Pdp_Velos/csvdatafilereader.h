#ifndef CSVDATAFILEREADER_H
#define CSVDATAFILEREADER_H

#include <QReadWriteLock>

#include "abstractdatafilereader.h"

namespace bss {
class CsvDataFileReader;
}

struct Trip;
struct Station;
class CsvDataFileReader : public AbstractDataFileReader
{
public:
    CsvDataFileReader(const DataFileParams& params);
    virtual ~CsvDataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations,
                                      QVector<Trip>& trips) const;

    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const;

};

#endif // CSVDATAFILEREADER_H
