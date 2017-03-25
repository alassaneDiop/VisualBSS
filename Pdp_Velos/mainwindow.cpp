#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QtMath>
#include <QPoint>

#include "config.h"
#include "data.h"
#include "trip.h"
#include "station.h"
#include "tripsselector.h"
#include "matrixglwidget.h"
#include "tripsverticesbuilder.h"
#include "glyphsverticesbuilder.h"
#include "stationsverticesbuilder.h"

const QVector<bss::SortParam> MainWindow::SORT_PARAMS = QVector<bss::SortParam>(
{ bss::DISTANCE, bss::DURATION, bss::ARRIVALS, bss::DEPARTURES, bss::CYCLES });

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_view(new Ui::MainWindow)
{
    /* If the signal is queued, the parameters must be of types that are known to Qt's meta-object system,
     * because Qt needs to copy the arguments to store them in an event behind the scenes.
     * */
    qRegisterMetaType<QVector<Trip>>("QVector<Trip>");
    qRegisterMetaType<QVector<Station>>("QVector<Station>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QVector<float>>("QVector<float>");
    qRegisterMetaType<QVector<QVector<int>>>("QVector<QVector<int>>");

    const Qt::ConnectionType connectionType = (Qt::ConnectionType)(Qt::QueuedConnection | Qt::UniqueConnection);
    connect(this, &MainWindow::stationsOrderChanged, this, &MainWindow::onStationsOrderChanged, connectionType);
    connect(this, &MainWindow::selectionChanged, this, &MainWindow::onSelectionChanged, connectionType);
    connect(this, &MainWindow::tripsChanged, this, &MainWindow::onTripsChanged, connectionType);
    connect(this, &MainWindow::tripsVerticesBuilt, this, &MainWindow::onTripsVerticesBuilt, connectionType);
    connect(this, &MainWindow::glyphsVerticesBuilt, this, &MainWindow::onGlyphsVerticesBuilt, connectionType);

    m_view->setupUi(this);
    m_view->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_view->actionClose_all->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));

    m_view->listView_top->setStyleSheet("QListView::item:hover{background-color:#a8f21f;}");
    m_view->listView_last->setStyleSheet("QListView::item:hover{background-color:#a8f21f;}");

    connect(m_view->actionOpen, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    connect(m_view->actionClose_all, &QAction::triggered, this, &MainWindow::on_action_closeAll_triggered);

    connect(m_view->timelinematrixwidget, &MatrixGLWidget::selectionChanged, this, &MainWindow::onMatrixSelectionChanged);

    m_model = new Data();
    connect(m_model, &Data::dataLoaded, this, &MainWindow::onDataLoaded, connectionType);
    connect(m_model, &Data::failedToLoadData, this, &MainWindow::onFailedToLoadData, connectionType);
    connect(m_model, &Data::dataUnloaded, this, &MainWindow::onDataUnloaded, connectionType);

    m_asyncTaskMonitor = new QFutureWatcher<void>();
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted, connectionType);
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished, connectionType);

    const QObject* distanceRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_distance->rootObject());
    connect(distanceRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_distance_firstValueChanged(qreal)));
    connect(distanceRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_distance_secondValueChanged(qreal)));
    connect(distanceRangeSlider, SIGNAL(firstPositionChanged(qreal)), SLOT(on_rangeSlider_distance_firstPositionChanged(qreal)));
    connect(distanceRangeSlider, SIGNAL(secondPositionChanged(qreal)), SLOT(on_rangeSlider_distance_secondPositionChanged(qreal)));

    const QObject* durationRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_duration->rootObject());
    connect(durationRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_duration_firstValueChanged(qreal)));
    connect(durationRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_duration_secondValueChanged(qreal)));
    connect(durationRangeSlider, SIGNAL(firstPositionChanged(qreal)), SLOT(on_rangeSlider_duration_firstPositionChanged(qreal)));
    connect(durationRangeSlider, SIGNAL(secondPositionChanged(qreal)), SLOT(on_rangeSlider_duration_secondPositionChanged(qreal)));

    const QObject* tripsFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_tripsFlow->rootObject());
    connect(tripsFlowRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_tripsFlow_firstValueChanged(qreal)));
    connect(tripsFlowRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_tripsFlow_secondValueChanged(qreal)));
    connect(tripsFlowRangeSlider, SIGNAL(firstPositionChanged(qreal)), SLOT(on_rangeSlider_tripsFlow_firstPositionChanged(qreal)));
    connect(tripsFlowRangeSlider, SIGNAL(secondPositionChanged(qreal)), SLOT(on_rangeSlider_tripsFlow_secondPositionChanged(qreal)));

    QObject* const directionRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_direction->rootObject());
    connect(directionRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_direction_firstValueChanged(qreal)));
    connect(directionRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_direction_secondValueChanged(qreal)));
    connect(directionRangeSlider, SIGNAL(firstPositionChanged(qreal)), SLOT(on_rangeSlider_direction_firstPositionChanged(qreal)));
    connect(directionRangeSlider, SIGNAL(secondPositionChanged(qreal)), SLOT(on_rangeSlider_direction_secondPositionChanged(qreal)));

    directionRangeSlider->blockSignals(true);
    directionRangeSlider->setProperty("snap", "NoSnap");
    directionRangeSlider->setProperty("stepSize", "0.1");
    directionRangeSlider->setProperty("from", 0);
    directionRangeSlider->setProperty("to", 360);
    directionRangeSlider->blockSignals(false);

    m_tripsDisplayParams.shouldShowArrivals = m_view->checkBox_showArrivals->isChecked();
    m_tripsDisplayParams.shouldShowCycles = m_view->checkBox_showCycles->isChecked();
    m_tripsDisplayParams.shouldShowDepartures = m_view->checkBox_showDepartures->isChecked();
    m_tripsDisplayParams.shouldShowDistance = m_view->checkBox_showDistance->isChecked();

    m_tripsFilterParams.maxDirection = 360;
    m_tripsFilterParams.minDirection = 0;

    m_stationsSortParam = SORT_PARAMS.at(m_view->comboBox_order->currentIndex());
}

MainWindow::~MainWindow()
{
    delete m_asyncTaskMonitor;
    delete m_model;
    delete m_view;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!m_canApplicationExit)
        event->ignore();

    QMainWindow::closeEvent(event);
}



template<typename T>
void MainWindow::runAsync(const QFuture<T>& future)
{
    m_asyncTaskMonitor->waitForFinished();
    m_asyncTaskMonitor->setFuture(future);
}



bool MainWindow::loadDataFromFiles(const QStringList& filenames, const bool& parallel)
{
    bool hasLoadedData = false;
    for (const QString filename : filenames)
        if (m_model->load(filename, parallel))
            hasLoadedData = true;

    return hasLoadedData;
}



void MainWindow::filterTrips(const QVector<Trip>& trips, const TripsFilterParams& params)
{
    const TripsFilter filter = TripsFilter(params);
    const QVector<Trip> filteredTrips = filter.filter(trips);
    const QVector<int> filteredTripsIds = ids(filteredTrips);
    if (m_tripsIds.toList().toSet() != filteredTripsIds.toList().toSet())
    {
        m_tripsIds = filteredTripsIds;
        emit tripsChanged(filteredTripsIds);
    }
}

void MainWindow::filterStations(const QVector<Station>& stations, const StationsFilterParams& params)
{
    const StationsFilter filter = StationsFilter(params);
    const QVector<Station> filteredStations = filter.filter(stations);
    const QVector<int> filteredStationsIds = ids(filteredStations);
    if (m_stationsIds.toList().toSet() != filteredStationsIds.toList().toSet())
        sortStations(filteredStations, m_stationsSortParam);
}

void MainWindow::sortStations(const QVector<Station>& stations, const bss::SortParam& param)
{
    const StationsSorter sorter = StationsSorter(param);
    const QVector<Station> sortedStations = sorter.sort(stations);
    const QVector<int> sortedStationsIds = ids(sortedStations);
    if (m_stationsIds != sortedStationsIds)
    {
        m_stationsIds = sortedStationsIds;
        emit stationsOrderChanged(sortedStationsIds);
    }
}

void MainWindow::selectTrips(const int& fromHour, const int& toHour,
                             const int& fromStationIndex, const int& toStationIndex)
{
    TripsSelectionParams selectionParams;
    selectionParams.fromHour = fromHour;
    selectionParams.toHour = toHour;
    selectionParams.stationsIds = m_stationsIds.mid(fromStationIndex, fromStationIndex + toStationIndex);

    const TripsSelector selector = TripsSelector(selectionParams);
    const QVector<Trip> selection = selector.selectFrom(m_model->trips(m_tripsIds));
    const QVector<int> selectionIds = ids(selection);
    if (m_selection.toList().toSet() != selectionIds.toList().toSet())
    {
        m_selection = selectionIds;
        emit selectionChanged(selectionIds);
    }
}



void MainWindow::drawSelectedTripsOnMap(const QVector<Trip>& selection)
{
    QVector<Trip> arrivals;
    QVector<Trip> departures;
    QVector<Trip> cycles;

    for (const Trip t : selection)
    {
        if (t.isCyclic && m_tripsDisplayParams.shouldShowCycles)
            cycles.append(t);
        else
        {
            for (const int stationId : m_stationsIds)
            {
                if ((stationId == t.startStationId) && m_tripsDisplayParams.shouldShowDepartures)
                    departures.append(t);

                if ((stationId == t.endStationId) && m_tripsDisplayParams.shouldShowArrivals)
                    arrivals.append(t);
            }
        }
    }

    buildTripsVertices(arrivals, departures, cycles);
}

void MainWindow::drawGlyphsOnMatrix(const QVector<Station>& stations, const TripsDisplayParams& params)
{
    const int glyphsCount = (bss::NB_OF_HOURS * stations.count());

    QVector<QVector<Trip>> arrivals;
    arrivals.reserve(glyphsCount);
    if (!params.shouldShowArrivals)
        arrivals.fill(QVector<Trip>());
    else
    {
        for (const Station s : stations)
        {
            for (int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
            {
                const auto filter = [this, &hour](const int& id)
                { return (m_model->trip(id).startDateTime.time().hour() == hour); };
                arrivals.append(m_model->trips(QtConcurrent::blockingFiltered(s.arrivalsIds, filter)));
            }
        }
    }

    QVector<QVector<Trip>> departures;
    departures.reserve(glyphsCount);
    if (!params.shouldShowDepartures)
        departures.fill(QVector<Trip>());
    else
    {
        for (const Station s : stations)
        {
            for (int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
            {
                const auto filter = [this, &hour](const int& id)
                { return (m_model->trip(id).startDateTime.time().hour() == hour); };
                departures.append(m_model->trips(QtConcurrent::blockingFiltered(s.departuresIds, filter)));
            }
        }
    }

    QVector<QVector<Trip>> cycles;
    cycles.reserve(glyphsCount);
    if (!params.shouldShowCycles)
        cycles.fill(QVector<Trip>());
    else
    {
        for (const Station s : stations)
        {
            for (int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
            {
                const auto filter = [this, &hour](const int& id)
                { return (m_model->trip(id).startDateTime.time().hour() == hour); };
                cycles.append(m_model->trips(QtConcurrent::blockingFiltered(s.cyclesIds, filter)));
            }
        }
    }

    buildGlyphsVertices(arrivals, departures, cycles, params.shouldShowDistance);
}



void MainWindow::buildStationsVertices(const QVector<Station>& stations)
{
    const StationsVerticesBuilder stationsVerticesBuilder;
    const QVector<float> stationsVertices = stationsVerticesBuilder.build(stations);
    emit stationsVerticesBuilt(stationsVertices);
}

void MainWindow::buildTripsVertices(const QVector<Trip>& arrivals,
                                    const QVector<Trip>& departures,
                                    const QVector<Trip>& cycles)
{
    const TripsVerticesBuilder tripsVerticesBuilder;
    const QVector<Station> stations = m_model->stations(m_stationsIds);
    const QVector<float> tripsVertices = tripsVerticesBuilder.build(stations, arrivals, departures, cycles);
    emit tripsVerticesBuilt(tripsVertices);
}

void MainWindow::buildGlyphsVertices(const QVector<QVector<Trip>>& arrivals,
                                     const QVector<QVector<Trip>>& departures,
                                     const QVector<QVector<Trip>>& cycles,
                                     const bool& showDistance)
{
    const GlyphsVerticesBuilder glyphsVerticesBuilder;
    const int height = m_view->timelinematrixwidget->height();
    const int width = m_view->timelinematrixwidget->width();
    const QVector<float> glyphsVertices = glyphsVerticesBuilder.build(height, width, arrivals, departures, cycles, showDistance);
    emit glyphsVerticesBuilt(glyphsVertices);
}



QVector<QDate> MainWindow::dates(const QVector<Trip>& trips)
{
    QVector<QDate> dates;
    for (const Trip t : trips)
    {
        if (!dates.contains(t.startDateTime.date()))
            dates.append(t.startDateTime.date());

        if (!dates.contains(t.endDateTime.date()))
            dates.append(t.endDateTime.date());
    }

    return dates;
}

qint64 MainWindow::maxDuration(const QVector<Trip>& trips)
{
    qint64 maxDuration = 0;
    for (const Trip t : trips)
        maxDuration = qMax(maxDuration, t.duration);

    return maxDuration;
}

qint64 MainWindow::maxDistance(const QVector<Trip>& trips)
{
    qint64 maxDistance = 0;
    for (const Trip t : trips)
        maxDistance = qMax(maxDistance, t.distance);

    return maxDistance;
}

int MainWindow::maxTripsFlow(const QVector<Station>& stations)
{
    int maxFlow = 0;
    for (const Station s : stations)
        maxFlow = qMax(maxFlow, s.tripsFlow);

    return maxFlow;
}

QVector<int> MainWindow::ids(const QVector<Trip>& trips)
{
    int (*returnId)(const Trip& t) = [](const Trip& t) { return t.id; };
    return QtConcurrent::blockingMapped(trips, returnId);
}

QVector<int> MainWindow::ids(const QVector<Station>& stations)
{
    int (*returnId)(const Station& s) = [](const Station& s) { return s.id; };
    return QtConcurrent::blockingMapped(stations, returnId);
}



void MainWindow::onAsyncTaskStarted()
{
    m_canApplicationExit = false;
    m_view->menuBar->setEnabled(false);
    m_view->frame_controls->setEnabled(false);
}

void MainWindow::onAsyncTaskFinished()
{
    m_canApplicationExit = true;
    m_view->menuBar->setEnabled(m_shouldEnableMenuBar);
    m_view->frame_controls->setEnabled(m_shouldEnableControls);
}



void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    qDebug() << "onDataLoaded" << "Trip number" << trips.count() << "Station number" << stations.size();
    m_shouldEnableControls = true;

    m_dates = dates(trips);
    m_tripsFilterParams.fromPeriod = m_dates.first();
    m_tripsFilterParams.toPeriod = m_dates.last();
    m_tripsFilterParams.maxDistance = maxDistance(trips);
    m_tripsFilterParams.maxDuration = maxDuration(trips);
    m_stationsFilterParams.maxTripsFlow = maxTripsFlow(stations);

    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, stations, m_stationsFilterParams));
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, trips, m_tripsFilterParams));

    QString (*dateToString)(const QDate& d) = [](const QDate& d) { return d.toString("dd/MM/yyyy"); };
    const QStringList datesStrings = QStringList::fromVector(QtConcurrent::blockingMapped(m_dates, dateToString));

    m_view->comboBox_fromPeriod->blockSignals(true);
    m_view->comboBox_fromPeriod->clear();
    m_view->comboBox_fromPeriod->addItems(datesStrings);
    m_view->comboBox_fromPeriod->setCurrentIndex(0);
    m_view->comboBox_fromPeriod->blockSignals(false);

    m_view->comboBox_toPeriod->blockSignals(true);
    m_view->comboBox_toPeriod->clear();
    m_view->comboBox_toPeriod->addItems(datesStrings);
    m_view->comboBox_toPeriod->setCurrentIndex(datesStrings.size() - 1);
    m_view->comboBox_toPeriod->blockSignals(false);

    QObject* const distanceRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_distance->rootObject());
    distanceRangeSlider->blockSignals(true);
    distanceRangeSlider->setProperty("to", m_tripsFilterParams.maxDistance);
    distanceRangeSlider->blockSignals(false);

    QObject* const durationRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_duration->rootObject());
    durationRangeSlider->blockSignals(true);
    durationRangeSlider->setProperty("to", m_tripsFilterParams.maxDuration);
    durationRangeSlider->blockSignals(false);

    QObject* const tripsFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_tripsFlow->rootObject());
    tripsFlowRangeSlider->blockSignals(true);
    tripsFlowRangeSlider->setProperty("to", m_stationsFilterParams.maxTripsFlow);
    tripsFlowRangeSlider->blockSignals(false);

    m_view->lineEdit_maxDistance->setText(QString::number(m_tripsFilterParams.maxDistance));
    m_view->lineEdit_maxDuration->setText(QString::number(m_tripsFilterParams.maxDuration));
    m_view->lineEdit_maxTripsFlow->setText(QString::number(m_stationsFilterParams.maxTripsFlow));
}

void MainWindow::onFailedToLoadData(const QString& filename, const QString& errorDesc)
{
    Q_UNUSED(filename);
    Q_UNUSED(errorDesc);
    // TODO : onFailedToLoadData
}

void MainWindow::onDataUnloaded()
{
    m_selection.clear();
    onSelectionChanged(m_selection);

    m_tripsIds.clear();
    onTripsChanged(m_tripsIds);

    m_stationsIds.clear();
    onStationsOrderChanged(m_stationsIds);

    m_shouldEnableControls = false;
    m_view->listView_top->setModel(nullptr);
    m_view->listView_last->setModel(nullptr);
}



void MainWindow::onTripsChanged(const QVector<int>& trips)
{
    Q_UNUSED(trips);
    runAsync(QtConcurrent::run(this, &MainWindow::drawGlyphsOnMatrix, m_model->stations(m_stationsIds), m_tripsDisplayParams));
}

void MainWindow::onSelectionChanged(const QVector<int>& selection)
{
    if (selection.isEmpty())
        buildStationsVertices(m_model->stations(m_stationsIds));
    else
        runAsync(QtConcurrent::run(this, &MainWindow::drawSelectedTripsOnMap, m_model->trips(selection)));
}

void MainWindow::onStationsOrderChanged(const QVector<int>& stationsIds)
{
    const QVector<Station> stations = m_model->stations(stationsIds);
    runAsync(QtConcurrent::run(this, &MainWindow::drawGlyphsOnMatrix, stations, m_tripsDisplayParams));

    if (m_selection.isEmpty())
        buildStationsVertices(stations);
    else
    {
        m_selection.clear();
        m_selection.squeeze();
        onSelectionChanged(m_selection);
    }

    const QVector<int> topStationsIds = stationsIds.mid(0, bss::RANK_SIZE);
    QStringList topStationsNames;
    topStationsNames.reserve(topStationsIds.size());
    for (const int id : topStationsIds)
        topStationsNames.append(m_model->station(id).name);

    const int lastStart = qMax(0, (stationsIds.size() - bss::RANK_SIZE));
    const QVector<int> lastStationsIds = stationsIds.mid(lastStart, lastStart + bss::RANK_SIZE);
    QStringList lastStationsNames;
    lastStationsNames.reserve(lastStationsIds.size());
    for (const int id : lastStationsIds)
        lastStationsNames.prepend(m_model->station(id).name);

    m_view->listView_top->setModel(new QStringListModel(topStationsNames));
    m_view->listView_last->setModel(new QStringListModel(lastStationsNames));
}

void MainWindow::onHighlightChanged(const int& highlight)
{
    Q_UNUSED(highlight);
    // TODO : onHighlightChanged
}

void MainWindow::onTripsDisplayParamsChanged(const TripsDisplayParams &params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::drawGlyphsOnMatrix, m_model->stations(m_stationsIds), params));
}

void MainWindow::onTripsFilterParamsChanged(const TripsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, m_model->trips(), params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortParam& param)
{
    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, m_model->stations(m_stationsIds), param));
}

void MainWindow::onStationsFilterParamsChanged(const StationsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, m_model->stations(), params));
}

void MainWindow::onMatrixSelectionChanged(const int& fromHour, const int& toHour,
                                          const int& fromStationIndex, const int& toStationIndex)
{
    runAsync(QtConcurrent::run(this, &MainWindow::selectTrips, fromHour, toHour, fromStationIndex, toStationIndex));
}



void MainWindow::onStationsVerticesBuilt(const QVector<float> stationsVertices)
{
    const int vertexSize = 5; // 2 for position x/y and 3 for color R/G/B
    m_view->mapwidget->loadStationsData(stationsVertices, stationsVertices.size() / vertexSize);
    m_view->mapwidget->centerView(stationsVertices);
}

void MainWindow::onTripsVerticesBuilt(const QVector<float> tripsVertices)
{
    const int vertexSize = 5; // 2 for position x/y and 3 for color R/G/B
    m_view->mapwidget->loadTripsData(tripsVertices, tripsVertices.size() / vertexSize);
    m_view->mapwidget->centerView(tripsVertices);
}

void MainWindow::onGlyphsVerticesBuilt(const QVector<float> glyphsVertices)
{
    const int vertexSize = 5; // 2 for position x/y and 3 for color R/G/B
    m_view->timelinematrixwidget->loadGlyphsData(glyphsVertices, glyphsVertices.size() / vertexSize);
}



void MainWindow::on_action_open_triggered()
{
    const QString caption = "Choose files to open";
    const QString filter = "*.csv *.xml *.json";
    const QString dirPath = QDir::rootPath();
    const QStringList filenames = QFileDialog::getOpenFileNames(this, caption, dirPath, filter);
    runAsync(QtConcurrent::run(this, &MainWindow::loadDataFromFiles, filenames, bss::PARALLEL_PARSING));
}

void MainWindow::on_action_closeAll_triggered()
{
    runAsync(QtConcurrent::run(m_model, &Data::unload));
}

void MainWindow::on_comboBox_fromPeriod_currentIndexChanged(int index)
{
    m_tripsFilterParams.fromPeriod = m_dates.at(index);
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_comboBox_toPeriod_currentIndexChanged(int index)
{
    m_tripsFilterParams.toPeriod = m_dates.at(index);
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_checkBox_showArrivals_stateChanged(int arg1)
{
    m_tripsDisplayParams.shouldShowArrivals = (arg1 == Qt::Checked);
    onTripsDisplayParamsChanged(m_tripsDisplayParams);
}

void MainWindow::on_checkBox_showDepartures_stateChanged(int arg1)
{
    m_tripsDisplayParams.shouldShowDepartures = (arg1 == Qt::Checked);
    onTripsDisplayParamsChanged(m_tripsDisplayParams);
}

void MainWindow::on_checkBox_showCycles_stateChanged(int arg1)
{
    m_tripsDisplayParams.shouldShowCycles = (arg1 == Qt::Checked);
    onTripsDisplayParamsChanged(m_tripsDisplayParams);
}

void MainWindow::on_checkBox_showDistance_stateChanged(int arg1)
{
    m_tripsDisplayParams.shouldShowDistance = (arg1 == Qt::Checked);
    onTripsDisplayParamsChanged(m_tripsDisplayParams);
}

void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    m_stationsSortParam = SORT_PARAMS.at(index);
    onStationsSorterParamChanged(m_stationsSortParam);
}

void MainWindow::on_rangeSlider_distance_firstValueChanged(qreal v)
{
    m_view->lineEdit_minDistance->setText(QString::number(v));
    m_tripsFilterParams.minDistance = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_distance_secondValueChanged(qreal v)
{
    m_view->lineEdit_maxDistance->setText(QString::number(v));
    m_tripsFilterParams.maxDistance = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_distance_firstPositionChanged(qreal p)
{
    m_view->lineEdit_minDistance->setText(QString::number(p));
}

void MainWindow::on_rangeSlider_distance_secondPositionChanged(qreal p)
{
    m_view->lineEdit_maxDistance->setText(QString::number(p));
}

void MainWindow::on_rangeSlider_duration_firstValueChanged(qreal v)
{
    m_view->lineEdit_minDuration->setText(QString::number(v));
    m_tripsFilterParams.minDuration = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_duration_secondValueChanged(qreal v)
{
    m_view->lineEdit_maxDuration->setText(QString::number(v));
    m_tripsFilterParams.maxDuration = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_duration_firstPositionChanged(qreal p)
{
    m_view->lineEdit_minDuration->setText(QString::number(p));
}

void MainWindow::on_rangeSlider_duration_secondPositionChanged(qreal p)
{
    m_view->lineEdit_maxDuration->setText(QString::number(p));
}

void MainWindow::on_rangeSlider_direction_firstValueChanged(qreal v)
{
    m_view->lineEdit_minDirection->setText(QString::number(v));
    m_tripsFilterParams.minDirection = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_direction_secondValueChanged(qreal v)
{
    m_view->lineEdit_maxDirection->setText(QString::number(v));
    m_tripsFilterParams.maxDirection = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_direction_firstPositionChanged(qreal p)
{
    m_view->lineEdit_minDirection->setText(QString::number(p));
}

void MainWindow::on_rangeSlider_direction_secondPositionChanged(qreal p)
{
    m_view->lineEdit_maxDirection->setText(QString::number(p));
}

void MainWindow::on_rangeSlider_tripsFlow_firstValueChanged(qreal v)
{
    m_view->lineEdit_minTripsFlow->setText(QString::number(v));
    m_stationsFilterParams.minTripsFlow = v;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}

void MainWindow::on_rangeSlider_tripsFlow_secondValueChanged(qreal v)
{
    m_view->lineEdit_maxTripsFlow->setText(QString::number(v));
    m_stationsFilterParams.maxTripsFlow = v;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}

void MainWindow::on_rangeSlider_tripsFlow_firstPositionChanged(qreal v)
{
    m_view->lineEdit_minTripsFlow->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_tripsFlow_secondPositionChanged(qreal v)
{
    m_view->lineEdit_maxTripsFlow->setText(QString::number(v));
}
