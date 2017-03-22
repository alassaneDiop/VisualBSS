#ifndef XMLDATAFILEREADER_H
#define XMLDATAFILEREADER_H

#include "abstractdatafilereader.h"

#include <QString>
#include <QReadWriteLock>


namespace bss {
class XmlDataFileReader;
}


class XmlDataFileReader : public AbstractDataFileReader
{
public:
    XmlDataFileReader(const DataFileParams& params);
    virtual ~XmlDataFileReader();
    virtual DataFileReadInfo readData(QHash<QString, Station>& stations, QVector<Trip>& trips) const override;

    virtual DataFileReadInfo parallelReadData(QHash<QString, Station>& stations,
                                              QVector<Trip>& trips,
                                              QReadWriteLock& stationsLock,
                                              QReadWriteLock& tripsLock) const;
};

#endif // XMLDATAFILEREADER_H
