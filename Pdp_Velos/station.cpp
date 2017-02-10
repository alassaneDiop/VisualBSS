#include "station.h"

Station::Station(const QString& name, const qreal latitude, const qreal longitude) :
    m_name(QString(name)),
    m_latitude(latitude),
    m_longitude(longitude)
{
    m_isValid = isValid(*this);
    m_isNull = isNull(*this);
}

Station::operator QString() const
{
    QString result;
    result += "latitude : " + QString:: number(getLatitude()) + '\t';
    result += "longitude : " + QString:: number(getLongitude()) + '\t';
    result += "name : " + getName();
    return result;
}

bool Station::isValid(const Station& s)
{
    return !isNull(s);
}

bool Station::isNull(const Station& s)
{
    return s.getName().isEmpty();
}
