#ifndef STATION_H
#define STATION_H

#include <QVector>
#include <QString>


namespace bss {
struct Station;
}


struct Trip;

/**
 * The Station struct contains all the data to be filtered, displayed and rendered.
 * A bike station has a world position expressed by its latitude and longitude.
 * It has unique name, and is associated to trips by their ids.
 */
struct Station
{
    inline bool operator ==(const Station& other) { return (id == other.id); }

    /**
     * @brief distance The distance in meters from this station to the specified one.
     * @param to The station to calculate the distance to.
     * @return The distance in meters.
     */
    qreal distance(const Station& to) const;

    /**
     * @brief direction The azimuth in degrees from this station to the specified one.
     * @param to The station to calculate the distance to.
     * @return The azimuth in degrees.
     */
    qreal direction(const Station& to) const;

    /// Associates the spcified arriving trip to this station.
    void appendArrival(const Trip& trip);

    /// Associates the spcified departing trip to this station.
    void appendDeparture(const Trip& trip);

    /// Associates the spcified cycling trip to this station.
    void appendCycle(const Trip& trip);

    /// Updates <i>tripsFlow</i>, <i>avgTripDistance</i> and <i>avgTripDuration</i>.
    void updateAppend(const Trip& trip);

    int id =  -1;                   /// Station unique id. (<i>-1</i> by default)
    QString name;                   /// Station unique name.

    qreal latitude = 0;             /// in degrees (<i>0</i> by default)
    qreal longitude = 0;            /// in degrees (<i>0</i> by default)

    qint64 avgTripDuration = 0;     /// Average duration of the associated trips (<i>0</i> by default)
    qreal avgTripDistance = 0;      /// Average distance of the associated trips (<i>0</i> by default)
    int tripsFlow = 0;              /// Total count of the associated trips (<i>0</i> by default)

    QVector<int> arrivalsIds;       /// Associated trips ids that arrived to this station
    QVector<int> departuresIds;     /// Associated trips ids that departed from this station
    QVector<int> cyclesIds;         /// Associated trips ids that departed from and arrived to this station
};

#endif // STATION_H
