#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <QString>
#include <QVector>

#include "trip.h"
#include "station.h"


namespace bss {
class DataFileReader;
struct DataFileParams;
struct DataFileReadInfo;
}

struct DataFileParams
{
    QString filename;
    Qt::DateFormat dateFormat = Qt::ISODate;
};

struct DataFileReadInfo
{
    bool ok = true;
    QString errorString;
};

class DataFileReader
{
public:
    DataFileReader(const DataFileParams& params);
    DataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);

    virtual ~DataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;
    virtual DataFileReadInfo readCsvData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;
    virtual DataFileReadInfo readXmlData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;
    virtual DataFileReadInfo readJsonData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;

    inline DataFileParams params() const { return m_params; }

private:
    DataFileParams m_params;
};

#endif // DATAFILEREADER_H
