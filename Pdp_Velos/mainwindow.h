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

/**
 * @brief The TripsDisplayParams struct contains display flags
 * for the timeline matrix and the map rendering.
 * Display is parameterized according to the trip type (arrival, departure or cycle).
 * The <i>showDistance</i> flag indicates wheter or not
 * trips should be rendered relatively to the maximum trip distance.
 * All diplsay flags are set to <i>true</i> by default.
 */
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

/// The MainWindow makes the view interact with the model through the Qt's signals system.
class MainWindow : public QMainWindow
{
    /// order corresponds to UI's combobox
    static const QVector<bss::SortParam> SORT_PARAMS;

    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:
    /**
     * Runs an asynchronous task (threading is managed internally by Qt).
     * When an asynchronous task is running, it is not possible to exit the program nor use control widgets.
     * Only one asynchronous task can run at a time. If a second task need be ran, the caller will have to wait.
     * @param The task to monitor.
     * @see QFutureWatcher<T>
     */
    template<typename T>
    void runAsync(const QFuture<T>& future);

    /**
     * Tries to open files from the specified filenames.
     * @param filenames The names of the files to open (extensions should be .csv, .xml or .json).
     * @param parallel Wheter or not execute a parallelized version of file parsing.
     * @return <i>true</i> if at least one file has been opened, <i>false</i> otherwise.
     * @see Model::loadData
     */
    bool loadDataFromFiles(const QStringList& filenames, const bool& parallel);

    /**
     * Filters the specified trips according to the specified filtering parameters.
     * If the newly filtered trips are differents from the current ones, then the timeline matrix and the selection should be refreshed.
     * @param trips The trips to filter.
     * @param params The filtering parameters.
     * @see TripsFilter::filter
     */
    void filterTrips(const QVector<Trip>& trips, const TripsFilterParams& params);

    /**
     * Filters the specified stations according to the specified filtering parameters.
     * If the newly filtered stations are differents from the current ones, then they should be sorted.
     * If the newly sorted stations are differents from the current ones, then the timeline matrix should be refreshed,
     * and if there is no selected trips, then the map should also be refreshed.
     * @param stations The stations to filter.
     * @param params The filtering parameters.
     * @see StationsFilter::filter
     */
    void filterStations(const QVector<Station>& stations, const StationsFilterParams& params);

    /**
     * Sorts the specified stations according to the spcified sorting parameter.
     * If the newly sorted stations are differents from the current ones, then the timeline matrix should be refreshed,
     * and if there is no selected trips, then the map should also be refreshed.
     * @param stations The stations to sort.
     * @param param The sorting parameter.
     * @see StationsSorter::sort
     */
    void sortStations(QVector<Station>& stations, const bss::SortParam& param);


    /**
     * Selects trips for which departure hour is in between <i>fromHour</i> and <i>toHour</i>, and for which departure and arrival stations indices (not their ids) are in
     * between <i>fromStationIndex</i> and <i>toStationIndex</i>.
     * @param fromHour
     * @param toHour
     * @param fromStationIndex
     * @param toStationIndex
     * @see TripsSelector::select
     */
    void selectTrips(const int& fromHour, const int& toHour,
                     const int& fromStationIndex, const int& toStationIndex);

    // TODO : find an appropriate name for this function
    void prepareToDrawSelectionOnMap(const QVector<bss::tripId>& selection);

    // TODO : find an appropriate name for this function
    void prepareToDrawTripsOnMatrix(const QVector<bss::stationId>& stations, const TripsDisplayParams& params);

    // TODO : find an appropriate name for this function
    void drawStationsOnMap(const QVector<bss::stationId>& stationsIds);

    // TODO : find an appropriate name for this function
    void drawSelectedTripsOnMap(const QVector<bss::tripId>& arrivalsIds,
                                const QVector<bss::tripId>& departuresIds,
                                const QVector<bss::tripId>& cyclesIds);

    // TODO : find an appropriate name for this function
    void drawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                           const QVector<QVector<bss::tripId>>& departuresIds,
                           const QVector<QVector<bss::tripId>>& cyclesIds,
                           const bool& showDistance);

    // TODO : find an appropriate place for these functions
    QVector<bss::stationId> ids(const QVector<Station>& stations);
    QVector<bss::tripId> ids(const QVector<Trip>& trips);

    /// Returns all different departure and arrival dates from a set of trips.
    static QVector<QDate> dates(const QVector<Trip>& trips);
    static quint64 maxDuration(const QVector<Trip>& trips);
    static int maxDistance(const QVector<Trip>& trips);
    static int maxFlow(const QVector<Station>& stations);



    /// A flag to indicate wheter or not the application can exit.
    /// This flag is set to <i>true</i> when an asynchronous has started running,
    /// and reset to <i>false</i> when it has finised. <i>true</i> by default.
    bool m_canApplicationExit = true;

    bool m_shouldEnableMenuBar = true;
    bool m_shouldEnableControls = false;

    bss::SortParam m_stationsSortParam;
    TripsDisplayParams m_tripsDisplayParams;
    TripsFilterParams m_tripsFilterParams;
    StationsFilterParams m_stationsFilterParams;

    /// Pointer to the object that contains all the widgets.
    Ui::MainWindow* m_view;

    /// Pointer to the object that manages the data.
    Model* m_model = nullptr;

    /// This object is used to monitor asynchronous tasks.
    /// There can only be one running async task at a time.
    /// If a second task needs be run, the caller will have to wait.
    QFutureWatcher<void>* m_asyncTaskMonitor = nullptr;

    /// Stores all the different dates retrieved from the trips.
    QVector<QDate> m_dates;

    /// Ids of trips that are filtered and rendered on the timeline matrix.
    QVector<bss::tripId> m_tripsIds;

    /// Ids of trips that are filtered, selected and rendered on the map..
    QVector<bss::tripId> m_selection;

    /// Ids of the stations that are filtered and sorted.
    QVector<bss::stationId> m_stationsIds;

    /// Id of the currently highlighted station (= -1 if there is none).
    bss::stationId m_highlight = (bss::stationId) -1;

    // TODO: Séb : mettre ou tu veux
    QVector<float> drawTrip(bss::tripId id, float posX, float posY, const QVector<float>& color);
    QVector<float> drawGlyph(const QVector<bss::tripId>& trips, float posX, float posY, const QVector<float>& color);

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
