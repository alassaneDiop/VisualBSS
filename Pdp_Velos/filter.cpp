//#include "filter.h"


//Filter::Filter(QObject *parent) : QObject(parent)
//{
//    showIncomingTrips = true;
//    showOutgoingTrips = true;
//    showCyclicTrips = true;
//    showTripsDistances = true;
//    order = bss::Order::CYCLIC_TRIPS;
//    /**********************/
//}

//QSet<const Trip*> Filter::apply(const QSet<const Trip*> data) const
//{
//    QSet<const Trip*> filteredData;
//    for (auto trip : data)
//    {
//        if (trip != nullptr)
//        {
//            bool matchesCriteria = true;
//            matchesCriteria &= trip->getDirection() <= getMaxDirection();
//            matchesCriteria &= trip->getDirection() >= getMinDirection();
//            matchesCriteria &= trip->getDistance() <= getMaxDistance();
//            matchesCriteria &= trip->getDistance() >= getMinDistance();
//            matchesCriteria &= trip->getDuration() <= getMaxDuration();
//            matchesCriteria &= trip->getDuration() >= getMinDuration();
//            /****************************************/

//            if (matchesCriteria)
//                filteredData += trip;
//        }
//    }
//    return filteredData;
//}

//bool Filter::setDate(const QDate& date)
//{
//    if (!date.isValid())
//        return false;
//    else
//    {
//        if (date != getDate())
//        {
//            this->date = date;
//            emit changed();
//        }
//        return true;
//    }
//}

//bool Filter::setShowIncomingTrips(const bool showIncomingTrips)
//{
//    if (this->showIncomingTrips != showIncomingTrips)
//    {
//        this->showIncomingTrips = showIncomingTrips;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setShowOutgoingTrips(const bool showOutgoingTrips)
//{
//    if (this->showOutgoingTrips != showOutgoingTrips)
//    {
//        this->showOutgoingTrips = showOutgoingTrips;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setShowCyclicTrips(const bool showCyclicTrips)
//{
//    if (this->showCyclicTrips != showCyclicTrips)
//    {
//        this->showCyclicTrips = showCyclicTrips;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setShowTripsDistances(const bool showTripsDistances)
//{
//    if (this->showTripsDistances != showTripsDistances)
//    {
//        this->showTripsDistances = showTripsDistances;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setOrder(const bss::Order order)
//{
//    if (this->order != order)
//    {
//        this->order = order;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setMaxDuration(QTime maxDuration)
//{
//    if (!maxDuration.isValid())
//        return false;
//    else
//    {
//        if (maxDuration < getMinDuration())
//            maxDuration = getMinDuration();

//        if (maxDuration != getMaxDuration())
//        {
//            this->maxDuration = maxDuration;
//            emit changed();
//        }
//        return true;
//    }
//}

//bool Filter::setMinDuration(QTime minDuration)
//{
//    if (!minDuration.isValid())
//        return false;
//    else
//    {
//        if (minDuration > getMaxDuration())
//            minDuration = getMaxDuration();

//        if (this->minDuration != minDuration)
//        {
//            this->minDuration = minDuration;
//            emit changed();
//        }
//        return true;
//    }
//}

//bool Filter::setMaxDistance(qreal maxDistance)
//{
//    maxDistance = qMin<qreal>(maxDistance, getMinDistance());
//    if (!qFuzzyCompare(maxDistance, getMaxDistance()))
//    {
//        this->maxDistance = maxDistance;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setMinDistance(qreal minDistance)
//{
//    minDistance = qBound<qreal>(0, minDistance, getMaxDistance());
//    if (!qFuzzyCompare(minDistance, getMinDistance()))
//    {
//        this->minDistance = minDistance;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setMaxDirection(qreal maxDirection)
//{
//    maxDirection = qMin<qreal>(maxDirection, getMinDirection());
//    if (!qFuzzyCompare(maxDirection, getMaxDirection()))
//    {
//        this->maxDirection = maxDirection;
//        emit changed();
//    }
//    return true;
//}

//bool Filter::setMinDirection(qreal minDirection)
//{
//    minDirection = qBound<qreal>(0, minDirection, getMaxDirection());
//    if (!qFuzzyCompare(minDirection, getMinDirection()))
//    {
//        this->minDirection = minDirection;
//        emit changed();
//    }
//    return true;
//}
