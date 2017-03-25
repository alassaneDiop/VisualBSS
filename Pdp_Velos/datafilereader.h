#ifndef DATAFILEPARSER_H
#define DATAFILEPARSER_H

#include "abstractdatafilereader.h"

#include <QReadWriteLock>

/**
 * @brief The DataFileReader class parses trips and stations from a specified filename.
 */
class DataFileReader : public AbstractDataFileReader
{
public:
    DataFileReader(const DataFileParams& params);
    DataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);

    virtual ~DataFileReader();

    /**
     * Parse trips and stations from a file.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const;

    /**
     * A paralellized version of @see DataFileReader::readData.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const;
};

#endif // DATAFILEPARSER_H
