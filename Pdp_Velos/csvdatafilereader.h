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
    CsvDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat);
    virtual ~CsvDataFileReader();
    virtual bool readData(QVector<Trip>& trips, QVector<Station>& stations) const override;
};

#endif // CSVDATAFILEREADER_H
