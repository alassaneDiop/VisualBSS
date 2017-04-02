#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <QString>
#include <QVector>
#include <QReadWriteLock>

namespace bss {
class AbstractDataFileReader;
struct DataFileParams;
struct DataFileReadInfo;
}

/**
 * @brief The DataFileParams struct contains parsing parameters.
 * More precisely, it contains the name of the file to open and
 * the format of the dates contained in the file.
 * By default, the filename is null and date format is @see Qt::ISODate.
 */
struct DataFileParams
{
    QString filename;
    Qt::DateFormat dateFormat = Qt::ISODate;
};


/**
 * @brief The DataFileReadInfo struct contains the information returned by
 * a data reading operation. More precisely, it contains a boolean flag (= <i>true</i> by default)
 * and an string to stored a verbose error message (if there is one).
 * @see AbstractDataFileReader::readData
 */
struct DataFileReadInfo
{
    bool ok = true;         /// wheter or not the data reading is a success (<i>true</i> by default)
    QString errorString;    /// the error message (if there is one)
};

struct Trip;
struct Station;

/**
 * This object is used by the @see Model class to parse data from a file.
 */
class AbstractDataFileReader
{
public:
    AbstractDataFileReader(const DataFileParams& params);
    AbstractDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);

    virtual ~AbstractDataFileReader();

    /**
     * Parse trips and stations from a file.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations,
                                      QVector<Trip>& trips) const = 0;

    /**
     * A paralellized version of @see AbstractDataFileReader::readData.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const = 0;

    inline DataFileParams params() const { return m_params; }

private:
    DataFileParams m_params;
};

#endif // DATAFILEREADER_H
