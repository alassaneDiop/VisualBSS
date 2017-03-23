#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFutureWatcher>
#include <QDate>

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
class Model;
class MainWindow : public QMainWindow
{
    // order corresponds to UI's combobox
    static const QVector<bss::SortParam> SORT_PARAMS;

    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:  
    template<typename T>
    void runAsync(const QFuture<T>& future);

    bool loadDataFromFiles(const QStringList& filenames, const bool& parallel);

    void filterTrips(const QVector<Trip>& trips, const TripsFilterParams& params);
    void filterStations(const QVector<Station>& stations, const StationsFilterParams& params);
    void sortStations(QVector<Station>& stations, const bss::SortParam& param);
    void selectTrips(const int& fromHour, const int& toHour,
                     const int& fromStationIndex, const int& toStationIndex);

    void prepareToDrawSelectionOnMap(const QVector<bss::tripId>& selection);
    void prepareToDrawTripsOnMatrix(const QVector<bss::stationId>& stations, const TripsDisplayParams& params);

    // affiche sur la map les stations
    void drawStationsOnMap(const QVector<bss::stationId>& stationsIds);

    // affiche sur la map les trajets filtres et selectionnes par l'utilisateur
    void drawSelectedTripsOnMap(const QVector<bss::tripId>& arrivalsIds,
                                const QVector<bss::tripId>& departuresIds,
                                const QVector<bss::tripId>& cyclesIds);

    // affiche sur la matrice les trajets nouvellement filtres
    void drawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                           const QVector<QVector<bss::tripId>>& departuresIds,
                           const QVector<QVector<bss::tripId>>& cyclesIds,
                           const bool& showDistance);



    QVector<bss::stationId> ids(const QVector<Station>& stations);
    QVector<bss::tripId> ids(const QVector<Trip>& trips);
    QVector<Station> stations(const QVector<bss::stationId>& ids);
    QVector<Trip> trips(const QVector<bss::tripId>& ids);

    static QVector<QDate> dates(const QVector<Trip>& trips);
    static quint64 maxDuration(const QVector<Trip>& trips);
    static int maxDistance(const QVector<Trip>& trips);
    static int maxFlow(const QVector<Station>& stations);


    bool m_canApplicationExit = true;
    bool m_shouldEnableMenuBar = true;
    bool m_shouldEnableControls = false;

    bss::SortParam m_stationsSortParam;
    TripsDisplayParams m_tripsDisplayParams;
    TripsFilterParams m_tripsFilterParams;
    StationsFilterParams m_stationsFilterParams;

    Ui::MainWindow* m_view;
    Model* m_model = nullptr;
    QFutureWatcher<void>* m_asyncTaskMonitor = nullptr;    // an object used to monitor threads

    QVector<QDate> m_dates;
    QVector<bss::tripId> m_tripsIds;                    // filtered trips ids
    QVector<bss::tripId> m_selection;                   // currently selected trips ids
    QVector<bss::stationId> m_stationsIds;              // filtered and sorted trips ids
    bss::stationId m_highlight = (bss::stationId) -1;   // currently highlighted station's id (-1 if there is none)

    // TODO: Séb : mettre ou tu veux
    QVector<float> drawTrip(bss::tripId id, float posX, float posY, const QVector<float>& color);
    QVector<float> drawGlyph(const QVector<bss::tripId>& trips, float posX, float posY, const QVector<float>& color);
    //

private slots:
    void onAsyncTaskStarted();
    void onAsyncTaskFinished();

    void onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void onFailedToLoadData(const QString& filename, const QString& errorDesc);
    void onDataUnloaded();

    void onTripsChanged(const QVector<bss::tripId>& filteredTrips);
    void onStationsOrderChanged(const QVector<bss::stationId>& stationsOrder);
    void onSelectionChanged(const QVector<bss::tripId>& selection);
    void onHighlightChanged(const bss::stationId& highlight);
    void onTripsDisplayParamsChanged(const TripsDisplayParams &params);
    void onTripsFilterParamsChanged(const TripsFilterParams& params);
    void onStationsSorterParamChanged(const bss::SortParam& param);
    void onStationsFilterParamsChanged(const StationsFilterParams& params);

    void onMatrixSelectionChanged(const int& fromHour, const int& toHour,
                                  const int& fromStationIndex, const int& toStationIndex);

    void onReadyToDrawSelectionOnMap(const QVector<bss::tripId>& arrivalsIds,
                                     const QVector<bss::tripId>& departuresIds,
                                     const QVector<bss::tripId>& cyclesIds);

    void onReadyToDrawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                                    const QVector<QVector<bss::tripId>>& departuresIds,
                                    const QVector<QVector<bss::tripId>>& cyclesIds,
                                    const bool& showDistance);

    void on_action_open_triggered();
    void on_action_closeAll_triggered();

    void on_comboBox_fromPeriod_currentIndexChanged(int index);
    void on_comboBox_toPeriod_currentIndexChanged(int index);

    void on_checkBox_showArrivals_stateChanged(int arg1);
    void on_checkBox_showDepartures_stateChanged(int arg1);
    void on_checkBox_showCycles_stateChanged(int arg1);
    void on_checkBox_showDuration_stateChanged(int arg1);
    void on_checkBox_showDistance_stateChanged(int arg1);

    void on_comboBox_order_currentIndexChanged(int index);

    void on_rangeSlider_distance_firstValueChanged(qreal v);
    void on_rangeSlider_distance_secondValueChanged(qreal v);
    void on_rangeSlider_distance_firstPositionChanged(qreal p);
    void on_rangeSlider_distance_secondPositionChanged(qreal p);
    void on_rangeSlider_duration_firstValueChanged(qreal v);
    void on_rangeSlider_duration_secondValueChanged(qreal v);
    void on_rangeSlider_duration_firstPositionChanged(qreal p);
    void on_rangeSlider_duration_secondPositionChanged(qreal p);
    void on_rangeSlider_direction_firstValueChanged(qreal v);
    void on_rangeSlider_direction_secondValueChanged(qreal v);
    void on_rangeSlider_direction_firstPositionChanged(qreal p);
    void on_rangeSlider_direction_secondPositionChanged(qreal p);
    void on_rangeSlider_tripsFlow_firstValueChanged(qreal v);
    void on_rangeSlider_tripsFlow_secondValueChanged(qreal v);
    void on_rangeSlider_tripsFlow_firstPositionChanged(qreal p);
    void on_rangeSlider_tripsFlow_secondPositionChanged(qreal p);

signals:
    void tripsChanged(const QVector<bss::tripId>& trips);
    void stationsOrderChanged(const QVector<bss::stationId>& stationsOrder);
    void selectionChanged(const QVector<bss::tripId>& selection);
    void highlightChanged(const bss::stationId& highlight);
    void tripsFilterParamsChanged(const TripsFilterParams& params);
    void stationsSorterParamChanged(const bss::SortParam& param);

    void readyToDrawSelectionOnMap(const QVector<bss::tripId>& arrivalsIds,
                                   const QVector<bss::tripId>& departuresIds,
                                   const QVector<bss::tripId>& cyclesIds);

    void readyToDrawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                                  const QVector<QVector<bss::tripId>>& departuresIds,
                                  const QVector<QVector<bss::tripId>>& cyclesIds,
                                  const bool& showDistance);

};

#endif // MAINWINDOW_H
