#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QSet>
#include "trip.h"
#include "iterator"

namespace bss {
class Model;
}

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    bool loadData(const QString& filename);
    inline QList<Trip> getTrips() const { return m_trips.values(); }
    inline QSet<Trip>::const_iterator begin() const { return m_trips.begin(); }
    inline QSet<Trip>::const_iterator end() const { return m_trips.end(); }

private:
    QSet<Trip> m_trips;

signals:

public slots:
};

#endif // MODEL_H
