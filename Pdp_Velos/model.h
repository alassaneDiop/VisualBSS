#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>
#include <QReadWriteLock>

#include "trip.h"
#include "station.h"
#include "typedefs.h"
#include "datafilereader.h"

namespace bss {
class Model;
struct DataLoadResult;
}

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    /**
     * @return Wheter or not at least one call to <i>loadData</i> returned <i>true</i>.
     * <i>false</i> by default.
     */
    inline bool hasData() const { return m_hasData; }

    inline int tripsCount() const { return m_trips.count(); }
    inline int stationsCount() const { return m_stations.count(); }

    inline const Trip& trip(const bss::tripId& id) const { return m_trips.at((int) id); }
    inline const Station& station(const bss::stationId& id) const { return m_stations.at((int) id); }

    inline const QVector<Trip>& trips() const { return m_trips; }
    inline const QVector<Station>& stations() const { return m_stations; }

    QVector<Station> stations(const QVector<bss::stationId>& ids) const;
    QVector<Trip> trips(const QVector<bss::tripId>& ids) const;

    /**
     * Loads trips and stations from the specified file.
     * @param filename The file to load (extension should be .csv, .xml or .json).
     * @param parallel Wheter or not parse the data in parallel.
     * @return the result from a call to <i>DataFileReader::readData</i>.
     * @see <i>DataFileReader::readData</i>.
     */
    bool loadData(const QString& filename, const bool& parallel);

    /**
     * Clear all the data and set the <i>hasDataFlag</i> to false it is not.
     * @return <i>true</i> if there are no data loaded, <i>false</i> otherwise.
     */
    bool unloadData();

private:   
    bool m_hasData = false;

    QVector<Trip> m_trips;
    QVector<Station> m_stations;

signals:
    void dataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void failedToLoadData(const QString& filename, const QString& errorDesc);
    void dataUnloaded();
    void failedToUnloadData();
};

#endif // MODEL_H
