#ifndef CSVDATAFILEREADER_H
#define CSVDATAFILEREADER_H

#include "datafilereader.h"

namespace bss {
class CsvDataFileReader;
}

class CsvDataFileReader : public DataFileReader
{
public:
    CsvDataFileReader(const QString& m_filename);
    virtual int readData(QSet<Station>& stations, QSet<Trip>& trips) const override;

    static QDate parseDate(const QString& dateString);
    static QTime parseTime(const QString& timeString);
    static QDateTime parseDateTime(const QString& dateTimeString);

    static Station parseStation(const QString& stationName, const QString& latitudeString, const QString& longitudeString);
    static Trip parseTrip(QString line);
};

#endif // CSVDATAFILEREADER_H
