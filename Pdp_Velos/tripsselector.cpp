#include "tripsselector.h"

#include "trip.h"

TripsSelector::TripsSelector()
{

}

TripsSelector::TripsSelector(const SelectionParams& params) :
    m_params(params)
{

}



QSet<const Trip*> TripsSelector::selectTripsFrom(const QSet<const Trip*>& trips) const
{
    QSet<const Trip*> selection;
    for (const Trip* trip : trips)
    {
        if (trip)
        {
            /*const int tripStartHour = trip->getStartDateTime().time().hour();
            const int tripEndHour = trip->getEndDateTime().time().hour();
            if ((tripStartHour >= getParameters().startHour)
                    && (tripEndHour <= getParameters().endHour)
                    && (getParameters().stations.contains(trip->getEndStation())
                        || getParameters().stations.contains(trip->getStartStation())))
                    selection.insert(trip);*/
        }
    }
    return selection;
}
