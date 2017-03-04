#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "model.h"
#include "tripsfilter.h"
#include "tripsselector.h"
#include "stationsfilter.h"
#include "stationssorter.h"

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
    Ui::MainWindow* ui;

    Model* m_model;
    TripsFilter* m_filter;
    TripsSelector* m_selector;
    StationsFilter* m_stationsFilter;
    StationsSorter* m_stationsSorter;

    QVector<int> m_filteredTrips;   // id's of filtered trips
    QVector<int> m_selection;       // currently selected trips's id's
    int m_highlight;                // currently highlighted station's id (-1 if there is none)

private slots:
    void onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void onFailedToLoadData(const QString& filename);
    void onFilteredTripsChanged(const QVector<int>& filteredTrips);
    void onSelectionChanged(const QVector<int>& selection);
    void onHighlightChanged(const int& highlight);

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
