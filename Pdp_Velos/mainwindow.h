#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "model.h"
#include "tripsfilter.h"
#include "tripsselector.h"
#include "stationsfilter.h"
#include "stationssorter.h"
#include "typedefs.h"

#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    void filterTrips(const TripsFilterParams& params);
    void sortStations(const bss::SortOrder& orderParam);

    Ui::MainWindow* ui;

    Model* m_model = nullptr;
    TripsFilter* m_filter = nullptr;
    TripsSelector* m_selector = nullptr;
    StationsFilter* m_stationsFilter = nullptr;
    StationsSorter* m_stationsSorter = nullptr;

    QVector<bss::tripId> m_filteredTrips;           // id's of filtered trips
    QVector<bss::tripId> m_selection;               // currently selected trips's id's
    QVector<bss::stationId> m_stationsOrder;        // ids of stations, sorted by user's specification
    bss::stationId m_highlight = (bss::stationId) -1;    // currently highlighted station's id (-1 if there is none)

private slots:
    void onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void onFailedToLoadData(const QString& filename);
    void onFilteredTripsChanged(const QVector<bss::tripId>& filteredTrips);
    void onSelectionChanged(const QVector<bss::tripId>& selection);
    void onStationsOrderChanged(const QVector<bss::stationId>& stationsOrder);
    void onHighlightChanged(const bss::stationId& highlight);

    void on_comboBox_period_currentIndexChanged(int index);
    void on_lineEdit_day_editingFinished();
    void on_comboBox_dayOfWeek_currentIndexChanged(int index);
    void on_checkBox_arrivals_stateChanged(int arg1);
    void on_checkBox_departures_stateChanged(int arg1);
    void on_checkBox_cycles_stateChanged(int arg1);
    void on_checkBox_duration_stateChanged(int arg1);
    void on_checkBox_distance_stateChanged(int arg1);
    void on_comboBox_order_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H
