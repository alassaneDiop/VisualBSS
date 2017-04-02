#ifndef XMLDATAFILEREADER_H
#define XMLDATAFILEREADER_H

#include "abstractdatafilereader.h"

#include <QReadWriteLock>


namespace bss {
class XmlDataFileReader;
}


/**
 * @brief The XmlDataFileReader class parses trips and stations from an XML file.
 */
class XmlDataFileReader : public AbstractDataFileReader
{
public:
    XmlDataFileReader(const DataFileParams& params);
    XmlDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat = Qt::ISODate);

    virtual ~XmlDataFileReader();

    /**
     * Parse trips and stations from an XML file.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const override;

    /**
     * A paralellized version of @see XmlDataFileReader::readData.
     * @param stations Hash where stations are uniquely mapped to their own names. Note this function can only increase its size.
     * @param trips The trips container to fill in. Note this function can only increase its size.
     * @return A DataFileReadInfo structure. The <i>ok</i> flag is set accordingly to the success of the operation.
     */
    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const;
};

#endif // XMLDATAFILEREADER_H
