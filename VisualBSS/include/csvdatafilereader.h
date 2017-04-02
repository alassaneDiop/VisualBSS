#ifndef CSVDATAFILEREADER_H
#define CSVDATAFILEREADER_H

#include "abstractdatafilereader.h"


namespace bss {
class CsvDataFileReader;
}

struct Trip;
struct Station;

/**
 * @brief The CsvDataFileReader class is indirectly used by the @see Model class to parse
 * trips and stations from a csv file.
 */
class CsvDataFileReader : public AbstractDataFileReader
{
public:
    CsvDataFileReader(const DataFileParams& params);
    CsvDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);
    virtual ~CsvDataFileReader();

    /**
     * Parse trips and stations from a csv file.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations,
                                      QVector<Trip>& trips) const;

    /**
     * A paralellized version of @see CsvDataFileReader::readData.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const;

};

#endif // CSVDATAFILEREADER_H
