#include "stationsfilter.h"

StationsFilter::StationsFilter()
{

}

StationsFilter::StationsFilter(const FilterParams& params) :
    m_params(params)
{

}

QSet<const Station*> StationsFilter::filter(const QSet<const Station*>& stations) const
{
    // TODO : Filter::filter
    return stations;
}

void StationsFilter::filter(QSet<const Station*>& stations)
{
    // TODO : Filter::filter
}
