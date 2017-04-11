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

#include "tripsfilter.h"

#include <QtConcurrent>

#include "trip.h"
#include "data.h"

TripsFilter::TripsFilter(const TripsFilterParams& params) :
    m_params(params)
{

}

QVector<Trip> TripsFilter::filter(const QVector<Trip>& trips) const
{
    const auto filter = [this](const Trip& t) { return keep(t); };
    return QtConcurrent::blockingFiltered(trips, filter);
}

QVector<int> TripsFilter::filter(const QVector<int>& tripsIds, const Data& data) const
{
    const auto filter = [this, &data](const int& tripId) { return keep(data.trip(tripId)); };
    return QtConcurrent::blockingFiltered(tripsIds, filter);
}

bool TripsFilter::keep(const Trip& t) const
{
    return (t.startDateTime.date() >= params().fromPeriod)
        && (t.endDateTime.date() <= params().toPeriod)
        && (t.direction <= params().maxDirection)
        && (t.direction >= params().minDirection)
        && (t.distance <= params().maxDistance)
        && (t.distance >= params().minDistance)
        && (t.duration <= params().maxDuration)
        && (t.duration >= params().minDuration);
}
