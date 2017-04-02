#ifndef SELECTOR_H
#define SELECTOR_H

#include <QVector>


namespace bss {
class TripsSelector;
}

/**
 * @brief The TripsSelectionParams struct contains properties to select trips
 * from the timeline matrix. Trips are selected in this matrix according to
 * a departure hour interval and stations ids.
 * <i>fromHour</i> and <i>toHour</i> are set to 0 by default.
 */
struct TripsSelectionParams
{
    int fromHour = 0;
    int toHour = 0;
    QVector<int> stationsIds;
};


struct Trip;

/**
 * The TripsSelector class select trips from selection parameters.
 * @see TripsSelectionParams
 */
class TripsSelector
{
public:
    TripsSelector(const TripsSelectionParams& params);

    inline const TripsSelectionParams& params() const { return m_params; }
    inline void setParams(const TripsSelectionParams& params) { m_params = params; }

    /**
     * Selects trips from a specified original trips set, according to the selection parameters.
     * @param trips The original trips set form which the selection will be made.
     * @return The trips that match the selection parameters.
     */
    QVector<Trip> selectFrom(const QVector<Trip>& trips) const;

private:
    /// The parameters according to which trips will be selected.
    TripsSelectionParams m_params;
};

#endif // SELECTOR_H
