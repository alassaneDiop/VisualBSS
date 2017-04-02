#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFutureWatcher>
#include <QDate>

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
class Data;

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
    void sortStations(const QVector<Station>& stations, const bss::SortParam& param);


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


    void drawStationsOnMap(const QVector<Station>& stations);
    void drawSelectedTripsOnMap(const QVector<Trip>& selection);
    void drawGlyphsOnMatrix(const QVector<Station>& stations, const TripsDisplayParams& params);


    void buildStationsVertices(const QVector<Station>& stations);

    void buildTripsVertices(const QVector<Trip>& arrivals,
                                const QVector<Trip>& departures,
                                const QVector<Trip>& cycles);

    void buildGlyphsVertices(const QVector<QVector<Trip>>& arrivals,
                            const QVector<QVector<Trip>>& departures,
                            const QVector<QVector<Trip>>& cycles,
                            const bool& showDistance);


    /// Returns all different departure and arrival dates from a set of trips.
    static QVector<QDate> dates(const QVector<Trip>& trips);
    static qint64 maxDuration(const QVector<Trip>& trips);
    static qint64 maxDistance(const QVector<Trip>& trips);
    static int maxTripsFlow(const QVector<Station>& stations);

    static QVector<int> ids(const QVector<Trip>& trips);
    static QVector<int> ids(const QVector<Station>& stations);

    /**
     * A flag to indicate wheter or not the application can exit.
     * This flag is set to <i>true</i> when an asynchronous has started running,
     * and reset to <i>false</i> when it has finised. <i>true</i> by default.
     */
    bool m_canApplicationExit = true;

    bool m_shouldEnableMenuBar = true;
    bool m_shouldEnableControls = false;

    bss::SortParam m_stationsSortParam;
    TripsDisplayParams m_tripsDisplayParams;
    TripsFilterParams m_tripsFilterParams;
    StationsFilterParams m_stationsFilterParams;

    /**
     * This object is used to monitor asynchronous tasks.
     * There can only be one running async task at a time.
     * If a second task needs be run, the caller will have to wait.
     */
    QFutureWatcher<void>* m_asyncTaskMonitor = nullptr;

    Ui::MainWindow* m_view;         /// Pointer to the object that contains all the widgets.
    Data* m_data = nullptr;       /// Pointer to the object that manages the data.

    QVector<QDate> m_dates;         /// Stores all the different dates retrieved from the trips.
    QVector<int> m_tripsIds;        /// The ids of trips that are filtered and rendered on the timeline matrix.
    QVector<int> m_selection;       /// The ids of trips that are filtered, selected and rendered on the map..
    QVector<int> m_stationsIds;     /// The ids of stations that are filtered and sorted.
    int m_highlight = -1;           /// Id of the currently highlighted station (= -1 if there is none).

private slots:
    void onAsyncTaskStarted();
    void onAsyncTaskFinished();

    void onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations);
    void onFailedToLoadData(const QString& filename, const QString& errorDesc);
    void onDataCleared();

    void onTripsChanged(const QVector<int>& filteredTrips);
    void onStationsOrderChanged(const QVector<int>& stationsOrder);
    void onSelectionChanged(const QVector<int>& selection);
    void onHighlightChanged(const int& highlight);

    void onTripsDisplayParamsChanged(const TripsDisplayParams &params);
    void onTripsFilterParamsChanged(const TripsFilterParams& params);
    void onStationsSorterParamChanged(const bss::SortParam& param);
    void onStationsFilterParamsChanged(const StationsFilterParams& params);

    void onMatrixSelectionChanged(const int& fromHour, const int& toHour,
                                  const int& fromStationIndex, const int& toStationIndex);

    void onStationsVerticesBuilt(const QVector<float>& stationsVertices);
    void onTripsVerticesBuilt(const QVector<float>& tripsVertices);
    void onGlyphsVerticesBuilt(const QVector<float>& glyphsVertices);

    void on_action_open_triggered();
    void on_action_closeAll_triggered();

    void on_comboBox_fromPeriod_currentIndexChanged(int index);
    void on_comboBox_toPeriod_currentIndexChanged(int index);

    void on_checkBox_showArrivals_stateChanged(int arg1);
    void on_checkBox_showDepartures_stateChanged(int arg1);
    void on_checkBox_showCycles_stateChanged(int arg1);
    void on_checkBox_showDistance_stateChanged(int arg1);

    void on_comboBox_order_currentIndexChanged(int index);

    void on_spinBox_minDistance_valueChanged(int arg1);
    void on_spinBox_maxDistance_valueChanged(int arg1);
    void on_spinBox_minDuration_valueChanged(int arg1);
    void on_spinBox_maxDuration_valueChanged(int arg1);
    void on_doubleSpinBox_minDirection_valueChanged(double arg1);
    void on_doubleSpinBox_maxDirection_valueChanged(double arg1);
    void on_spinBox_minTripsFlow_valueChanged(int arg1);
    void on_spinBox_maxTripsFlow_valueChanged(int arg1);

signals:
    void tripsChanged(const QVector<int>& trips);
    void stationsOrderChanged(const QVector<int>& stationsOrder);
    void selectionChanged(const QVector<int>& selection);
    void highlightChanged(const int& highlight);
    void tripsFilterParamsChanged(const TripsFilterParams& params);
    void stationsSorterParamChanged(const bss::SortParam& param);

    void stationsVerticesBuilt(const QVector<float>& stationsVertices);
    void tripsVerticesBuilt(const QVector<float>& tripsVertices);
    void glyphsVerticesBuilt(const QVector<float>& glyphsVertices);
};

#endif // MAINWINDOW_H
