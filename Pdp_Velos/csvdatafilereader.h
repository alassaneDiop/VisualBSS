#ifndef CSVDATAFILEREADER_H
#define CSVDATAFILEREADER_H

#include "datafilereader.h"

namespace bss {
class CsvDataFileReader;
}


class Station;
class CsvDataFileReader : public DataFileReader
{
public:
    CsvDataFileReader(const QString& filename);
    virtual ~CsvDataFileReader();
    virtual DataFileReadInfo readData() const override;
};

#endif // CSVDATAFILEREADER_H
