/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

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
    connect(this, &MainWindow::stationsVerticesBuilt, this, &MainWindow::onStationsVerticesBuilt, connectionType);

    m_view->setupUi(this);
    m_view->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_view->actionClose_all->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));

    m_view->listView_top->setStyleSheet("QListView::item:hover{background-color:#a8f21f;}");
    m_view->listView_last->setStyleSheet("QListView::item:hover{background-color:#a8f21f;}");

    connect(m_view->actionOpen, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    connect(m_view->actionClose_all, &QAction::triggered, this, &MainWindow::on_action_closeAll_triggered);

    connect(m_view->timelinematrixwidget, &MatrixGLWidget::selectionChanged, this, &MainWindow::onMatrixSelectionChanged);

    m_data = new Data();
    connect(m_data, &Data::dataLoaded, this, &MainWindow::onDataLoaded, connectionType);
    connect(m_data, &Data::failedToLoadData, this, &MainWindow::onFailedToLoadData, connectionType);
    connect(m_data, &Data::dataCleared, this, &MainWindow::onDataCleared, connectionType);

    m_asyncTaskMonitor = new QFutureWatcher<void>();
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted, connectionType);
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished, connectionType);

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
    delete m_data;
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
        if (m_data->load(filename, parallel))
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
    const int len = (toStationIndex - fromStationIndex) + 1;

    TripsSelectionParams selectionParams;
    selectionParams.fromHour = fromHour;
    selectionParams.toHour = toHour;
    selectionParams.stationsIds = m_stationsIds.mid(fromStationIndex, len);

    const TripsSelector selector = TripsSelector(selectionParams);
    const QVector<Trip> selection = selector.selectFrom(m_data->trips(m_tripsIds));
    const QVector<int> selectionIds = ids(selection);
    if (m_selection.toList().toSet() != selectionIds.toList().toSet())
    {
        m_selection = selectionIds;
        emit selectionChanged(selectionIds);
    }
}



void MainWindow::drawStationsOnMap(const QVector<Station>& stations)
{
    buildStationsVertices(stations);
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
                { return (m_data->trip(id).startDateTime.time().hour() == hour); };
                arrivals.append(m_data->trips(QtConcurrent::blockingFiltered(s.arrivalsIds, filter)));
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
                { return (m_data->trip(id).startDateTime.time().hour() == hour); };
                departures.append(m_data->trips(QtConcurrent::blockingFiltered(s.departuresIds, filter)));
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
                { return (m_data->trip(id).startDateTime.time().hour() == hour); };
                cycles.append(m_data->trips(QtConcurrent::blockingFiltered(s.cyclesIds, filter)));
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
    const QVector<Station> stations = m_data->stations(m_stationsIds);
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
    QVector<int> ids;
    for (const Trip t : trips)
        ids.append(t.id);

    return ids;
}

QVector<int> MainWindow::ids(const QVector<Station>& stations)
{
    QVector<int> ids;
    for (const Station s : stations)
        ids.append(s.id);

    return ids;
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
    drawStationsOnMap(stations);

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

    m_view->spinBox_maxDistance->blockSignals(true);
    m_view->spinBox_maxDistance->setRange(0, m_tripsFilterParams.maxDistance);
    m_view->spinBox_maxDistance->blockSignals(false);

    m_view->spinBox_maxDuration->blockSignals(true);
    m_view->spinBox_maxDuration->setRange(0, m_tripsFilterParams.maxDuration);
    m_view->spinBox_maxDuration->blockSignals(false);

    m_view->spinBox_maxTripsFlow->blockSignals(true);
    m_view->spinBox_maxTripsFlow->setRange(0, m_stationsFilterParams.maxTripsFlow);
    m_view->spinBox_maxTripsFlow->blockSignals(false);
}

void MainWindow::onFailedToLoadData(const QString& filename, const QString& errorDesc)
{
    Q_UNUSED(filename);
    Q_UNUSED(errorDesc);
    // TODO : onFailedToLoadData
}

void MainWindow::onDataCleared()
{
    m_shouldEnableControls = false;

    m_selection.clear();
    m_tripsIds.clear();
    m_stationsIds.clear();

    m_view->listView_top->setModel(nullptr);
    m_view->listView_last->setModel(nullptr);

    m_view->mapwidget->clear();
    m_view->timelinematrixwidget->clear();
}



void MainWindow::onTripsChanged(const QVector<int>& trips)
{
    Q_UNUSED(trips);
    runAsync(QtConcurrent::run(this, &MainWindow::drawGlyphsOnMatrix, m_data->stations(m_stationsIds), m_tripsDisplayParams));
}

void MainWindow::onSelectionChanged(const QVector<int>& selection)
{
    if (selection.isEmpty())
        drawStationsOnMap(m_data->stations(m_stationsIds));
    else
        runAsync(QtConcurrent::run(this, &MainWindow::drawSelectedTripsOnMap, m_data->trips(selection)));
}

void MainWindow::onStationsOrderChanged(const QVector<int>& stationsIds)
{
    const QVector<Station> stations = m_data->stations(stationsIds);
    runAsync(QtConcurrent::run(this, &MainWindow::drawGlyphsOnMatrix, stations, m_tripsDisplayParams));

    if (m_selection.isEmpty())
        drawStationsOnMap(stations);
    else
    {
        m_selection.clear();
        onSelectionChanged(m_selection);
    }

    const QVector<int> topStationsIds = stationsIds.mid(0, bss::RANK_SIZE);
    QStringList topStationsNames;
    topStationsNames.reserve(topStationsIds.size());
    for (const int id : topStationsIds)
        topStationsNames.append(m_data->station(id).name);

    const int lastStart = qMax(0, (stationsIds.size() - bss::RANK_SIZE));
    const QVector<int> lastStationsIds = stationsIds.mid(lastStart, lastStart + bss::RANK_SIZE);
    QStringList lastStationsNames;
    lastStationsNames.reserve(lastStationsIds.size());
    for (const int id : lastStationsIds)
        lastStationsNames.prepend(m_data->station(id).name);

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
    runAsync(QtConcurrent::run(this, &MainWindow::drawGlyphsOnMatrix, m_data->stations(m_stationsIds), params));
}

void MainWindow::onTripsFilterParamsChanged(const TripsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, m_data->trips(), params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortParam& param)
{
    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, m_data->stations(m_stationsIds), param));
}

void MainWindow::onStationsFilterParamsChanged(const StationsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, m_data->stations(), params));
}

void MainWindow::onMatrixSelectionChanged(const int& fromHour, const int& toHour,
                                          const int& fromStationIndex, const int& toStationIndex)
{
    runAsync(QtConcurrent::run(this, &MainWindow::selectTrips, fromHour, toHour, fromStationIndex, toStationIndex));
}



void MainWindow::onStationsVerticesBuilt(const QVector<float>& stationsVertices)
{
    const int vertexSize = 5; // 2 for position x/y and 3 for color R/G/B
    m_view->mapwidget->loadStationsData(stationsVertices, stationsVertices.size() / vertexSize);
    m_view->mapwidget->centerView(stationsVertices);
}

void MainWindow::onTripsVerticesBuilt(const QVector<float>& tripsVertices)
{
    const int vertexSize = 5; // 2 for position x/y and 3 for color R/G/B
    m_view->mapwidget->loadTripsData(tripsVertices, tripsVertices.size() / vertexSize);
    m_view->mapwidget->centerView(tripsVertices);
}

void MainWindow::onGlyphsVerticesBuilt(const QVector<float>& glyphsVertices)
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
    runAsync(QtConcurrent::run(m_data, &Data::clear));
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

void MainWindow::on_spinBox_minDistance_valueChanged(int arg1)
{
    m_tripsFilterParams.minDistance = arg1;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_spinBox_maxDistance_valueChanged(int arg1)
{
    m_tripsFilterParams.maxDistance = arg1;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_spinBox_minDuration_valueChanged(int arg1)
{
    m_tripsFilterParams.minDuration = arg1;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_spinBox_maxDuration_valueChanged(int arg1)
{
    m_tripsFilterParams.minDistance = arg1;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_doubleSpinBox_minDirection_valueChanged(double arg1)
{
    m_tripsFilterParams.minDirection = arg1;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_doubleSpinBox_maxDirection_valueChanged(double arg1)
{
    m_tripsFilterParams.maxDirection = arg1;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_spinBox_minTripsFlow_valueChanged(int arg1)
{
    m_stationsFilterParams.minTripsFlow = arg1;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}

void MainWindow::on_spinBox_maxTripsFlow_valueChanged(int arg1)
{
    m_stationsFilterParams.maxTripsFlow = arg1;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}
