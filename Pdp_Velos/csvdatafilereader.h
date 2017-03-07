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
    virtual bool readData(QVector<Trip>& tripsData, QVector<Station>& stationsData) const override;

private:
    // TODO : retirer codage dans le dur du format du temps
    const QString dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
};

#endif // CSVDATAFILEREADER_H
