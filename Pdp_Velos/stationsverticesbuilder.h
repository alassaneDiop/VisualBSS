#ifndef STATIONSVERTICESBUILDER_H
#define STATIONSVERTICESBUILDER_H

#include <QVector>

struct Station;
class StationsVerticesBuilder
{
public:
    StationsVerticesBuilder();

    QVector<float> build(const QVector<Station>& stations) const;
};

#endif // STATIONSVERTICESBUILDER_H
