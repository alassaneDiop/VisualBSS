#include "datafilereader.h"

DataFileReader::DataFileReader(const QString& filename, const Qt::DateFormat& dateFormat) :
    m_filename(filename),
    m_dateFormat(dateFormat)
{

}

DataFileReader::~DataFileReader()
{

}
