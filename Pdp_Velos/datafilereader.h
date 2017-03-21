#ifndef DATAFILEPARSER_H
#define DATAFILEPARSER_H

#include "abstractdatafilereader.h"

class DataFileReader : public AbstractDataFileReader
{
public:
    DataFileReader(const DataFileParams& params);
    DataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);

    virtual ~DataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;

};

#endif // DATAFILEPARSER_H
