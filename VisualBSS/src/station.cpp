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

#include "station.h"

#include <QtMath>

#include "trip.h"


qreal Station::distance(const Station& to) const
{
    // Algorithm from http://www.movable-type.co.uk/scripts/latlong.html

    const qreal R = 6371000; // meters
    const qreal phi1 = qDegreesToRadians(latitude);
    const qreal phi2 = qDegreesToRadians(to.latitude);
    const qreal x = (to.longitude - longitude) * qCos((phi1 + phi2) / 2);
    const qreal y = (phi2 - phi1);
    return R * qSqrt(qPow(x, 2) + qPow(y, 2));
}

qreal Station::direction(const Station& to) const
{
    // https://en.wikipedia.org/wiki/Azimuth
    const float x = (to.longitude - longitude);
    const float y = (to.latitude - latitude);
    return qRadiansToDegrees(qAtan2(y, x)) + 180;
}

void Station::appendArrival(const Trip& trip)
{
    arrivalsIds.append(trip.id);
    updateAppend(trip);
}

void Station::appendDeparture(const Trip& trip)
{
    departuresIds.append(trip.id);
    updateAppend(trip);
}

void Station::appendCycle(const Trip& trip)
{
    cyclesIds.append(trip.id);
    updateAppend(trip);
}

void Station::updateAppend(const Trip& trip)
{
    tripsFlow++;

    const int totalDuration = ((tripsFlow - 1) * avgTripDuration) + trip.duration;
    avgTripDuration = (totalDuration / tripsFlow);

    const int totalDistance = ((tripsFlow - 1) * avgTripDistance) + trip.distance;
    avgTripDistance = (totalDistance / tripsFlow);
}
