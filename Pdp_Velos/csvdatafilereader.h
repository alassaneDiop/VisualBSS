#ifndef CSVDATAFILEREADER_H
#define CSVDATAFILEREADER_H

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
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;
};

#endif // CSVDATAFILEREADER_H
