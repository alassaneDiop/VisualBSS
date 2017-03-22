#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <QString>
#include <QVector>
#include <QReadWriteLock>


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

struct Trip;
struct Station;
class AbstractDataFileReader
{
public:
    AbstractDataFileReader(const DataFileParams& params);
    AbstractDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);

    virtual ~AbstractDataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations,
                                      QVector<Trip>& trips) const = 0;

    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const = 0;

    inline DataFileParams params() const { return m_params; }

private:
    DataFileParams m_params;
};

#endif // DATAFILEREADER_H
