#ifndef STATION_H
#define STATION_H

#include <QString>

namespace bss {
class Station;
}

class Station
{
public:   
    Station(const QString& name = QString(),
            const qreal latitude = 0,
            const qreal longitude = 0);

    inline bool isValid() const { return m_isValid; }
    inline bool isNull() const { return m_isNull; }

    inline QString getName() const { return m_name; }
    inline qreal getLatitude() const { return m_latitude; }
    inline qreal getLongitude() const { return m_longitude; }

    operator QString() const;
    bool operator ==(const Station& station) const;

    static bool isValid(const Station& s);
    static bool isNull(const Station& s);

private:
    bool m_isValid;
    bool m_isNull;

    QString m_name;
    qreal m_latitude;
    qreal m_longitude;

};

#endif // STATION_H
