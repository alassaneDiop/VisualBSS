#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QSet>

namespace bss {
class Model;
// TODO : enum SortOrder
enum SortOrder
{
    DEPARTURES,
    ARRIVALS,
    CYCLES,
    DISTANCE,
    DURATION
};
}

class Station;
class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject* parent = 0);

    inline QList<const Station*> getStations() const { return m_stations.values(); }
    bool loadData(const QString& filename);

private:
    QSet<const Station*> m_stations;

signals:
    void dataLoaded(const QList<const Station*>& stations);

public slots:

};

#endif // MODEL_H
