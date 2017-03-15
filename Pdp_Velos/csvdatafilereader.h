#ifndef CSVDATAFILEREADER_H
#define CSVDATAFILEREADER_H

#include "datafilereader.h"

namespace bss {
class CsvDataFileReader;
}

struct Trip;
struct Station;
class CsvDataFileReader : public DataFileReader
{
public:
    CsvDataFileReader(const DataFileParams& params);
    virtual ~CsvDataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;
};

#endif // CSVDATAFILEREADER_H
