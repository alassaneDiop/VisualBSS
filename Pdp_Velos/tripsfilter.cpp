#include "tripsfilter.h"


TripsFilter::TripsFilter()
{

}

TripsFilter::TripsFilter(const FilterParams& params) : m_params(params)
{

}

QSet<const Trip*> TripsFilter::filter(const QSet<const Trip*>& trips) const
{
    // TODO : Filter::filter
    return trips;
}

void TripsFilter::filter(QSet<const Trip*>& trips)
{
    // TODO : Filter::filter
}
