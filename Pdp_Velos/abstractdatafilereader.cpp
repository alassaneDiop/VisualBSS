#include "abstractdatafilereader.h"


AbstractDataFileReader::AbstractDataFileReader(const DataFileParams& params) :
    m_params(params)
{

}

AbstractDataFileReader::AbstractDataFileReader(const QString& filename, const Qt::DateFormat& dateFormat)
{
    m_params.filename = filename;
    m_params.dateFormat = dateFormat;
}

AbstractDataFileReader::~AbstractDataFileReader()
{

}

/*
 * Pour CitiBike:
 * "tripduration",
 * "starttime",
 * "stoptime",
 * "start station id",
 * "start station name",
 * "start station latitude",
 * "start station longitude",
 * "end station id",
 * "end station name",
 * "end station latitude",
 * "end station longitude",
*/
