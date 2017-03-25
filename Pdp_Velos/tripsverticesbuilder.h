#ifndef TRIPSVERTICESBUILDER_H
#define TRIPSVERTICESBUILDER_H

#include <QVector>

namespace bss {
class TripsVerticesBuilder;
}


struct Trip;
struct Station;
class TripsVerticesBuilder
{
public:
    TripsVerticesBuilder();

    QVector<float> build(const QVector<Station>& stations,
                         const QVector<Trip>& arrivals,
                         const QVector<Trip>& departures,
                         const QVector<Trip>& cycles) const;

};

#endif // TRIPSVERTICESBUILDER_H
