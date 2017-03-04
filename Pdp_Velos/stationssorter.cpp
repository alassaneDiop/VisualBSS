#include "stationssorter.h"


StationsSorter::StationsSorter(const bss::SortOrder& param) :
    m_sortParam(param)
{

}

QList<const Station*> StationsSorter::sort(const QSet<const Station*>& stations) const
{
    // TODO : sort
    return stations.toList();
}

void StationsSorter::sort(QSet<const Station*>& stations) const
{
    // TODO : sort
}
