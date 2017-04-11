#ifndef STATIONSVERTICESBUILDER_H
#define STATIONSVERTICESBUILDER_H

#include <QVector>

struct Station;
class Data;
class StationsVerticesBuilder
{
public:
    StationsVerticesBuilder();

    QVector<float> build(const QVector<int>& stationsIds, const Data& data) const;
};

#endif // STATIONSVERTICESBUILDER_H
