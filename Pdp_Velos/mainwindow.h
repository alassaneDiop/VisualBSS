#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFutureWatcher>

#include "model.h"
#include "typedefs.h"
#include "tripsfilter.h"
#include "stationsfilter.h"
#include "stationssorter.h"

namespace Ui {
class MainWindow;
}

struct Trip;
struct Station;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:
    template<typename T>
    void runAsync(const QFuture<T>& future);

    bool loadData(const QStringList& filenames);
    bool unloadData();

    void filterTrips(const TripsFilterParams& params);
    void filterStations(const StationsFilterParams& params);
    void sortStations(const bss::SortOrder& param, QVector<Station>& stations);

    void drawMap(const QVector<Station>& stations, const QVector<Trip>& arrivals, const QVector<Trip>& departures, const QVector<Trip>& cycles);
    void drawTimelineMatrix(const QHash<QPair<short, int>, QVector<Trip>>& arrivals,
                            const QHash<QPair<short, int>, QVector<Trip>>& departures,
                            const QHash<QPair<short, int>, QVector<Trip>>& cycles);


    bool m_canApplicationExit = true;
    bool m_shouldEnableMenuBar = true;
    bool m_shouldEnableControlsFrame = false;

    Ui::MainWindow* ui;
    Model* m_model = nullptr;
    bss::SortOrder m_sortOrder = bss::DISTANCE;
    TripsFilterParams m_tripsFilterParams;
    StationsFilterParams m_stationsFilterParams;
    QFutureWatcher<void>* m_futureWatcher = nullptr;

    QVector<bss::tripId> m_filteredTripsIds;
    QVector<bss::tripId> m_selection;                   // currently selected trips's id's
    QVector<bss::stationId> m_orderedStationsIds;       // ids of stations, sorted by user's specification
    bss::stationId m_highlight = (bss::stationId) -1;   // currently highlighted station's id (-1 if there is none)

private slots:
    void onAsyncTaskStarted();
    void onAsyncTaskFinished();

    void onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void onFailedToLoadData(const QString& filename, const QString& errorDesc);
    void onDataUnloaded();

    void onFilteredTripsChanged(const QVector<bss::tripId>& filteredTrips);
    void onStationsOrderChanged(const QVector<bss::stationId>& stationsOrder);
    void onSelectionChanged(const QVector<bss::tripId>& selection);
    void onHighlightChanged(const bss::stationId& highlight);
    void onTripsFilterParamsChanged(const TripsFilterParams& params);
    void onStationsSorterParamChanged(const bss::SortOrder& param);

    void on_action_open_triggered();
    void on_action_closeAll_triggered();
    void on_comboBox_period_currentIndexChanged(int index);
    void on_lineEdit_day_editingFinished();
    void on_comboBox_dayOfWeek_currentIndexChanged(int index);
    void on_checkBox_arrivals_stateChanged(int arg1);
    void on_checkBox_departures_stateChanged(int arg1);
    void on_checkBox_cycles_stateChanged(int arg1);
    void on_checkBox_duration_stateChanged(int arg1);
    void on_checkBox_distance_stateChanged(int arg1);
    void on_comboBox_order_currentIndexChanged(int index);
    void on_rangeSlider_distance_firstValueChanged(qreal v);
    void on_rangeSlider_distance_secondValueChanged(qreal v);
    void on_rangeSlider_duration_firstValueChanged(qreal v);
    void on_rangeSlider_duration_secondValueChanged(qreal v);
    void on_rangeSlider_direction_firstValueChanged(qreal v);
    void on_rangeSlider_direction_secondValueChanged(qreal v);
    void on_rangeSlider_odFlow_firstValueChanged(qreal v);
    void on_rangeSlider_odFlow_secondValueChanged(qreal v);

signals:
    void dataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void failedToLoadData(const QString& filename, const QString& errorDesc);
    void dataUnloaded();

    void filteredTripsChanged(const QVector<bss::tripId>& filteredTrips);
    void stationsOrderChanged(const QVector<bss::stationId>& stationsOrder);
    void selectionChanged(const QVector<bss::tripId>& selection);
    void highlightChanged(const bss::stationId& highlight);
    void tripsFilterParamsChanged(const TripsFilterParams& params);
    void stationsSorterParamChanged(const bss::SortOrder& param);
};

#endif // MAINWINDOW_H
