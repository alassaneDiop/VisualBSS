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

namespace bss {
struct TripsDisplayParams;
}

struct TripsDisplayParams
{
    bool shouldShowDistance = true;
    bool shouldShowArrivals = true;
    bool shouldShowDepartures = true;
    bool shouldShowCycles = true;
};

struct Trip;
struct Station;
class MainWindow : public QMainWindow
{
    typedef bss::tripId tripId;
    typedef bss::stationId stationId;

    // order corresponds to UI's combobox
    static const QVector<bss::SortOrder> SORT_PARAMS;

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

    // affiche sur la map les stations
    void drawStationsOnMap(const QVector<stationId>& stationsIds);

    // affiche sur la map les trajets filtres et selectionnes par l'utilisateur
    void drawSelectedTripsOnMap(const QVector<tripId>& arrivalsIds,
                                const QVector<tripId>& departuresIds,
                                const QVector<tripId>& cyclesIds);

    // affiche sur la matrice les trajets nouvellement filtres
    void drawFilteredTripsOnMatrix(const QVector<QVector<tripId>>& arrivalsIds,
                                   const QVector<QVector<tripId>>& departuresIds,
                                   const QVector<QVector<tripId>>& cyclesIds,
                                   const bool& showDistance);

    bool m_canApplicationExit = true;
    bool m_shouldEnableMenuBar = true;
    bool m_shouldEnableControlsFrame = false;

    bss::SortOrder m_sortOrder = bss::DISTANCE;
    TripsDisplayParams m_tripsDisplayParams;
    TripsFilterParams m_tripsFilterParams;
    StationsFilterParams m_stationsFilterParams;

    Ui::MainWindow* ui;
    Model* m_model = nullptr;
    QFutureWatcher<void>* m_asyncTaskMonitor = nullptr;    // an object used to monitor threads

    QVector<tripId> m_filteredTripsIds;
    QVector<tripId> m_selection;               // currently selected trips's id's
    QVector<stationId> m_orderedStationsIds;   // ids of stations, sorted by user's specification
    stationId m_highlight = (stationId) -1;    // currently highlighted station's id (-1 if there is none)

private slots:
    void onAsyncTaskStarted();
    void onAsyncTaskFinished();

    void onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void onFailedToLoadData(const QString& filename, const QString& errorDesc);
    void onDataUnloaded();

    void onFilteredTripsChanged(const QVector<tripId>& filteredTrips);
    void onStationsOrderChanged(const QVector<stationId>& stationsOrder);
    void onSelectionChanged(const QVector<tripId>& selection);
    void onHighlightChanged(const stationId& highlight);
    void onTripsFilterParamsChanged(const TripsFilterParams& params);
    void onStationsSorterParamChanged(const bss::SortOrder& param);
    void onStationsFilterParamsChanged(const StationsFilterParams& params);

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

    void filteredTripsChanged(const QVector<tripId>& filteredTrips);
    void stationsOrderChanged(const QVector<bss::stationId>& stationsOrder);
    void selectionChanged(const QVector<tripId>& selection);
    void highlightChanged(const stationId& highlight);
    void tripsFilterParamsChanged(const TripsFilterParams& params);
    void stationsSorterParamChanged(const bss::SortOrder& param);
};

#endif // MAINWINDOW_H
