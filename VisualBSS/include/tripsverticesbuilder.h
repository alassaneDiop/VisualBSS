#ifndef TRIPSVERTICESBUILDER_H
#define TRIPSVERTICESBUILDER_H

#include <QVector>

namespace bss {
class TripsVerticesBuilder;
}


struct Trip;
struct Station;
class Data;
class TripsVerticesBuilder
{
public:
    TripsVerticesBuilder();

    QVector<float> build(const QVector<int>& stationsIds,
                         const QVector<int>& arrivalsIds,
                         const QVector<int>& departuresIds,
                         const QVector<int>& cyclesIds,
                         const Data& data) const;

};

#endif // TRIPSVERTICESBUILDER_H
