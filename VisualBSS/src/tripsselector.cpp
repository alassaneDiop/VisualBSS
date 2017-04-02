/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include "tripsselector.h"

#include <QtConcurrent>

#include "trip.h"


TripsSelector::TripsSelector(const TripsSelectionParams& params) :
    m_params(params)
{

}



QVector<Trip> TripsSelector::selectFrom(const QVector<Trip>& trips) const
{
    const QTime fromTime = QTime(params().fromHour, 0, 0);
    const QTime toTime = QTime(params().toHour, 0, 0);

    const auto select = [this, &fromTime, &toTime](const Trip& t)
    {     
        return (t.startDateTime.time() >= fromTime)
            && (t.endDateTime.time() <= toTime)
            && ((params().stationsIds.contains(t.startStationId))
            || (params().stationsIds.contains(t.endStationId)));
    };

    return QtConcurrent::blockingFiltered(trips, select);
}
