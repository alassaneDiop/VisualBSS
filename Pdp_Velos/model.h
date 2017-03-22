#ifndef MODEL_H
#define MODEL_H

#include <QVector>

#include "trip.h"
#include "station.h"
#include "typedefs.h"
#include "datafilereader.h"

namespace bss {
class Model;
struct DataLoadResult;
}

struct DataLoadResult
{
    DataFileReadInfo info;
    QVector<Trip> trips;
    QVector<Station> stations;
};

class Model
{
public:
    inline bool hasData() const { return m_hasData; }

    inline int tripsCount() const { return m_trips.count(); }
    inline int stationsCount() const { return m_stations.count(); }

    inline const Trip& trip(const bss::tripId& id) const { return m_trips.at((int) id); }
    inline const Station& station(const bss::stationId& id) const { return m_stations.at((int) id); }

    inline const QVector<Trip>& trips() const { return m_trips; }
    inline const QVector<Station>& stations() const { return m_stations; }

    DataLoadResult loadData(const QString& filename);
    bool unloadData();

private:   
    bool m_hasData = false;
    QVector<Trip> m_trips;
    QVector<Station> m_stations;
};

#endif // MODEL_H
