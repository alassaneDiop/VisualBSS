#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QtMath>
#include <QPoint>

#include "config.h"
#include "model.h"
#include "trip.h"
#include "station.h"
#include "tripsselector.h"
#include "matrixglwidget.h"

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
    qRegisterMetaType<QVector<bss::tripId>>("QVector<bss::tripId>");
    qRegisterMetaType<QVector<bss::stationId>>("QVector<bss::stationId>");
    qRegisterMetaType<QVector<QVector<bss::tripId>>>("QVector<QVector<bss::tripId>>");

    const Qt::ConnectionType connectionType = (Qt::ConnectionType)(Qt::QueuedConnection | Qt::UniqueConnection);
    connect(this, &MainWindow::dataLoaded, this, &MainWindow::onDataLoaded, connectionType);
    connect(this, &MainWindow::failedToLoadData, this, &MainWindow::onFailedToLoadData, connectionType);
    connect(this, &MainWindow::dataUnloaded, this, &MainWindow::onDataUnloaded, connectionType);
    connect(this, &MainWindow::stationsOrderChanged, this, &MainWindow::onStationsOrderChanged, connectionType);
    connect(this, &MainWindow::selectionChanged, this, &MainWindow::onSelectionChanged, connectionType);
    connect(this, &MainWindow::tripsChanged, this, &MainWindow::onTripsChanged, connectionType);
    connect(this, &MainWindow::readyToDrawSelectionOnMap, this, &MainWindow::onReadyToDrawSelectionOnMap, connectionType);
    connect(this, &MainWindow::readyToDrawTripsOnMatrix, this, &MainWindow::onReadyToDrawTripsOnMatrix, connectionType);


    m_view->setupUi(this);
    m_view->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_view->actionClose_all->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
    m_view->widget_bottom->hide();

    connect(m_view->actionOpen, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    connect(m_view->actionClose_all, &QAction::triggered, this, &MainWindow::on_action_closeAll_triggered);

    connect(m_view->timelinematrixwidget, &MatrixGLWidget::onSelectionChanged, this, &MainWindow::onMatrixSelectionChanged);

    m_model = new Model();

    m_asyncTaskMonitor = new QFutureWatcher<void>();
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted, connectionType);
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished, connectionType);

    const QObject* distanceRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_distance->rootObject());
    connect(distanceRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_distance_firstValueChanged(qreal)));
    connect(distanceRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_distance_secondValueChanged(qreal)));

    const QObject* durationRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_duration->rootObject());
    connect(durationRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_duration_firstValueChanged(qreal)));
    connect(durationRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_duration_secondValueChanged(qreal)));

    const QObject* odFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_odFlow->rootObject());
    connect(odFlowRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_odFlow_firstValueChanged(qreal)));
    connect(odFlowRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_odFlow_secondValueChanged(qreal)));

    QObject* const directionRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_direction->rootObject());
    connect(directionRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_direction_firstValueChanged(qreal)));
    connect(directionRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_direction_secondValueChanged(qreal)));
    directionRangeSlider->setProperty("from", 0);
    directionRangeSlider->setProperty("to", 360);

    // TODO : SEB m_tripsFilterParams
    /*m_tripsFilterParams.period
    m_tripsFilterParams.day = QDate() ui->lineEdit_day->*/
    m_tripsFilterParams.maxDirection = 360;
    m_tripsFilterParams.minDirection = 0;

    m_tripsDisplayParams.shouldShowArrivals = m_view->checkBox_showArrivals->isChecked();
    m_tripsDisplayParams.shouldShowCycles = m_view->checkBox_showCycles->isChecked();
    m_tripsDisplayParams.shouldShowDepartures = m_view->checkBox_showDepartures->isChecked();
    m_tripsDisplayParams.shouldShowDistance = m_view->checkBox_showDistance->isChecked();

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


bool MainWindow::loadDataFromFile(const QString& filename, const bool& parallel)
{
    const DataLoadResult loadResult = m_model->loadData(filename, parallel);
    if (!loadResult.info.ok)
    {
        emit failedToLoadData(filename, loadResult.info.errorString);
        return false;
    }
    else
    {
        emit dataLoaded(loadResult.trips, loadResult.stations);
        return true;
    }
}

bool MainWindow::loadDataFromFiles(const QStringList& filenames, const bool& parallel)
{
    bool hasLoadedData = false;
    for (const QString filename : filenames)
        if (loadDataFromFile(filename, parallel))
            hasLoadedData = true;

    return hasLoadedData;
}

bool MainWindow::unloadData()
{
    if (!m_model->unloadData())
        return false;
    else
    {
        emit dataUnloaded();
        return true;
    }
}



void MainWindow::filterTrips(const QVector<Trip>& trips, const TripsFilterParams& params)
{
    const TripsFilter filter = TripsFilter(params);
    const QVector<Trip> filteredTrips = filter.filter(trips);
    const QVector<bss::tripId> tripsIds = ids(filteredTrips);
    if (m_tripsIds != tripsIds)
    {
        m_tripsIds = tripsIds;
        emit tripsChanged(tripsIds);
    }
}

void MainWindow::filterStations(const QVector<Station>& stations, const StationsFilterParams& params)
{
    const StationsFilter filter = StationsFilter(params);
    QVector<Station> filteredStations = filter.filter(stations);
    const QVector<bss::stationId> stationsIds = ids(filteredStations);
    if (m_stationsIds.toList().toSet() != stationsIds.toList().toSet())
        sortStations(filteredStations, m_stationsSortParam);
}

void MainWindow::sortStations(QVector<Station>& stations, const bss::SortParam& param)
{
    const StationsSorter sorter = StationsSorter(param);
    sorter.sort(stations);

    const QVector<bss::stationId> stationsOrder = ids(stations);
    if (m_stationsIds != stationsOrder)
    {
        m_stationsIds = stationsOrder;
        emit stationsOrderChanged(stationsOrder);
    }
}

void MainWindow::selectTrips(const char& startHour, const char& endHour,
                             const int& startStationIndex, const int& endStationIndex)
{
    TripsSelectionParams selectionParams;
    selectionParams.startHour = startHour;
    selectionParams.endHour = endHour;
    selectionParams.stations = m_stationsIds.mid(startStationIndex, startStationIndex + endStationIndex);

    const TripsSelector selector = TripsSelector(selectionParams);
    const QVector<bss::tripId> selection = ids(selector.select(trips(m_tripsIds)));
    if (m_selection.toList().toSet() != selection.toList().toSet())
    {
        m_selection = selection;
        emit selectionChanged(selection);
    }
}



void MainWindow::prepareToDrawSelectionOnMap(const QVector<bss::tripId>& selection)
{
    // TODO : to optimize if possible
    QVector<bss::tripId> arrivalsId;
    QVector<bss::tripId> departuresId;
    QVector<bss::tripId> cyclesId;

    for (const bss::stationId sId : m_stationsIds)
    {
        for (const bss::tripId tId : selection)
        {
            const Trip t = m_model->trip(tId);
            if (m_model->trip(tId).isCyclic && m_tripsDisplayParams.shouldShowCycles)
                cyclesId.append(tId);
            else
            {
                if ((sId == t.startStationId) && m_tripsDisplayParams.shouldShowDistance)
                    departuresId.append(tId);
                else if (m_tripsDisplayParams.shouldShowArrivals)
                    arrivalsId.append(tId);
            }
        }
    }

    emit readyToDrawSelectionOnMap(arrivalsId, departuresId, cyclesId);
}

void MainWindow::prepareToDrawTripsOnMatrix(const QVector<bss::stationId>& stations, const TripsDisplayParams& params)
{
    // TODO : to optimize if possible
    const int glyphsCount = bss::NB_OF_HOURS * stations.count();

    QVector<QVector<bss::tripId>> arrivalsIds;
    QVector<QVector<bss::tripId>> departuresIds;
    QVector<QVector<bss::tripId>> cyclesIds;

    arrivalsIds.reserve(glyphsCount);
    departuresIds.reserve(glyphsCount);
    cyclesIds.reserve(glyphsCount);

    for (const bss::stationId id : stations)
    {
        const Station s = m_model->station(id);
        for (int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
        {
            const auto filter = [this, &hour](const bss::tripId& id)
            { return (m_model->trip(id).startDateTime.time().hour() == hour); };

            if (params.shouldShowArrivals)
                arrivalsIds.append(QtConcurrent::blockingFiltered(s.arrivalsIds, filter));
            else
                arrivalsIds.append(QVector<bss::tripId>());

            if (params.shouldShowDepartures)
                departuresIds.append(QtConcurrent::blockingFiltered(s.departuresIds, filter));
            else
                departuresIds.append(QVector<bss::tripId>());

            if (params.shouldShowCycles)
                cyclesIds.append(QtConcurrent::blockingFiltered(s.cyclesIds, filter));
            else
                cyclesIds.append(QVector<bss::tripId>());
        }
    }

    emit readyToDrawTripsOnMatrix(arrivalsIds, departuresIds, cyclesIds, params.shouldShowDistance);
}



// affiche sur la map les stations
void MainWindow::drawStationsOnMap(const QVector<bss::stationId>& stationsIds)
{
    // x and y;
    const unsigned short positionTupleSize = 2;
    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> stationsVertices;
    const int stationsVerticesCount = stationsIds.size();
    stationsVertices.reserve(stationsVerticesCount * positionTupleSize * colorTupleSize);

    for (const bss::stationId id : stationsIds)
    {
        const float maxLongitude = 180.f;
        const float maxLatitude = 90.f;

        const Station s = m_model->station(id);
        stationsVertices.append(s.longitude / maxLongitude);
        stationsVertices.append(s.latitude / maxLatitude);

        stationsVertices += QVector<float>({ 1.f, 1.f, 0.3f });
    }

    m_view->mapwidget->loadStationsData(stationsVertices, stationsVerticesCount);
    m_view->mapwidget->centerView(stationsVertices);
}


// affiche sur la map les trajets filtres et selectionnes par l'utilisateur
void MainWindow::drawSelectedTripsOnMap(const QVector<bss::tripId>& arrivalsIds,
                                        const QVector<bss::tripId>& departuresIds,
                                        const QVector<bss::tripId>& cyclesIds)
{
    // x and y;
    const unsigned short positionTupleSize = 2;
    // 1 trip has 2 points (start/end)
    const unsigned short pointPerTrip = 2;
    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> tripsVertices;
    const int arrivalsVertices = arrivalsIds.size() * pointPerTrip * positionTupleSize * colorTupleSize;
    const int departuresVertices = departuresIds.size() * pointPerTrip * positionTupleSize * colorTupleSize;
    const int cyclesVertices = cyclesIds.size() * positionTupleSize * colorTupleSize;

    tripsVertices.reserve(arrivalsVertices + departuresVertices + cyclesVertices);

    int tripsVerticesCount = 0;

    // TODO: damien : renommer des variables
    const auto drawTrips = [this](  const QVector<bss::tripId>& ids,
                                QVector<float> tripsVertices,
                                int &verticesCount,
                                const QVector<float>& originColor,
                                const QVector<float>& destinationColor)
    {
        for (const bss::tripId arrivalId : ids)
        {
            const Trip trip = m_model->trip(arrivalId);

            const bss::stationId startStationId = trip.startStationId;
            const Station startStation = m_model->station(startStationId);

            const float maxLongitude = 180.f;
            const float maxLatitude = 90.f;

            tripsVertices.append((float) (startStation.longitude / maxLongitude));
            tripsVertices.append((float) (startStation.latitude / maxLatitude));

            tripsVertices += originColor;


            const bss::stationId endStationId = trip.endStationId;
            const Station endStation = m_model->station(endStationId);

            tripsVertices.append((float) (endStation.longitude / maxLongitude));
            tripsVertices.append((float) (endStation.latitude / maxLatitude));

            tripsVertices += destinationColor;

            verticesCount += 2;
        }
    };

    drawTrips(arrivalsIds, tripsVertices, tripsVerticesCount, bss::ARRIVAL_ORIGIN_COLOR, bss::ARRIVAL_DESTINATION_COLOR);
    drawTrips(departuresIds, tripsVertices, tripsVerticesCount, bss::DEPARTURE_ORIGIN_COLOR, bss::DEPARTURE_DESTINATION_COLOR);

    m_view->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
    m_view->mapwidget->centerView(tripsVertices);
}

// affiche sur la matrice les trajets nouvellement filtres
void MainWindow::drawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                                   const QVector<QVector<bss::tripId>>& departuresIds,
                                   const QVector<QVector<bss::tripId>>& cyclesIds,
                                   const bool& showDistance)
{
    Q_UNUSED(cyclesIds);
    Q_UNUSED(showDistance);

    // TODO: damien : renommer des variables
    const auto drawTrips = [this](  const QVector<QVector<bss::tripId>>& ids,
                                    QVector<float>& glyphsVertices,
                                    int &verticesCount,
                                    const QVector<float>& color)
    {
        const float width = m_view->timelinematrixwidget->width();
        const float heigth = m_view->timelinematrixwidget->height();
        const float intervalX = m_view->timelinematrixwidget->width() / (bss::NB_OF_HOURS * 1.f);
        const int glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

        int stationIndex = 0;
        for (const QVector<bss::tripId> arrivals : ids)
        {
            for (int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
            {
                for (const bss::tripId id : arrivals)
                {
                    const Trip t = m_model->trip(id);

                    // VERTEX #1
                    float positionX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                    float positionY = stationIndex * glyphIntervalY;

                    const float rotationCenterX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                    const float rotationCenterY = stationIndex * glyphIntervalY + bss::GLYPH_HEIGHT / 2;

                    const auto rotatePointAroundTarget = [](float angle, QPointF position, QPointF target)
                    {
                        const float sin = qSin(angle);
                        const float cos = qCos(angle);

                        float pX = target.x() - position.x();
                        float pY = target.y() - position.y();

                        float newX = pX * cos - pY * sin;
                        float newY = pX * sin + pY * cos;

                        newX += position.x();
                        newY += position.y();

                        return QPointF(newX, newY);
                    };

                    const float tripDirection = t.direction;

                    QPointF newP;
                    newP = rotatePointAroundTarget(tripDirection,
                                                   QPointF(positionX, positionY),
                                                   QPointF(rotationCenterX, rotationCenterY));

                    positionX = newP.x() / width * 2 - 1;
                    positionY = newP.y() / heigth * 2 - 1;

                    glyphsVertices += QVector<float>({ positionX, -positionY });
                    glyphsVertices += color;


                    // VERTEX #2
                    positionX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                    positionY = stationIndex * glyphIntervalY + bss::GLYPH_HEIGHT;

                    newP = rotatePointAroundTarget(tripDirection,
                                                   QPoint(positionX, positionY),
                                                   QPoint(rotationCenterX, rotationCenterY));

                    positionX = newP.x() / width * 2 - 1;
                    positionY = newP.y() / heigth * 2 - 1;

                    glyphsVertices += QVector<float>({ positionX, -positionY });
                    glyphsVertices += color;
                    verticesCount+= 2;
                }
            }
            stationIndex++;
        }
    };

    // x and y;
    const unsigned short positionTupleSize = 2;
    // 1 trip has 2 points (start/end)
    const unsigned short pointPerTrip = 2;
    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> glyphsVertices;
    int reserved = arrivalsIds.size() * bss::NB_OF_HOURS * pointPerTrip * colorTupleSize * positionTupleSize;
    reserved += departuresIds.size() * bss::NB_OF_HOURS * pointPerTrip * colorTupleSize * positionTupleSize;
    //reserved += cyclesIds.size() * bss::NB_OF_HOURS * colorTupleSize * positionTupleSize;
    glyphsVertices.reserve(reserved);

    int verticesCount = 0;

    drawTrips(arrivalsIds, glyphsVertices, verticesCount, bss::GLYPH_ARRIVAL_COLOR);
    drawTrips(departuresIds, glyphsVertices, verticesCount, bss::GLYPH_DEPARTURE_COLOR);

    m_view->timelinematrixwidget->loadGlyphsData(glyphsVertices, verticesCount);
}



QVector<bss::stationId> MainWindow::ids(const QVector<Station>& stations)
{
    bss::stationId (*returnId)(const Station& s) = [](const Station& s) { return s.id; };
    return QtConcurrent::blockingMapped(stations, returnId);
}

QVector<bss::tripId> MainWindow::ids(const QVector<Trip>& trips)
{
    bss::tripId (*returnId)(const Trip& t) = [](const Trip& t) { return t.id; };
    return QtConcurrent::blockingMapped(trips, returnId);
}

QVector<Station> MainWindow::stations(const QVector<bss::stationId>& ids)
{
    QVector<Station> stations;
    stations.reserve(ids.size());
    for (const bss::stationId id : ids)
        stations.append(m_model->station(id));

    return stations;
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

QVector<Trip> MainWindow::trips(const QVector<bss::tripId>& ids)
{
    QVector<Trip> trips;
    trips.reserve(ids.size());
    for (const bss::tripId id : ids)
        trips.append(m_model->trip(id));

    return trips;
}

quint64 MainWindow::maxDuration(const QVector<Trip>& trips)
{
    quint64 maxDuration = 0;
    for (const Trip t : trips)
        maxDuration = qMax(maxDuration, t.duration);

    return maxDuration;
}

quint64 MainWindow::minDuration(const QVector<Trip>& trips)
{
    quint64 minDuration = 0;
    for (const Trip t : trips)
        minDuration = qMin(minDuration, t.duration);

    return minDuration;
}

int MainWindow::maxDistance(const QVector<Trip>& trips)
{
    int maxDistance = 0;
    for (const Trip t : trips)
        maxDistance = qMax(maxDistance, t.distance);

    return maxDistance;
}

int MainWindow::minDistance(const QVector<Trip>& trips)
{
    int minDistance = 0;
    for (const Trip t : trips)
        minDistance = qMin(minDistance, t.distance);

    return minDistance;
}

int MainWindow::maxFlow(const QVector<Station>& stations)
{
    int maxFlow = 0;
    for (const Station s : stations)
        maxFlow = qMax(maxFlow, s.flow);

    return maxFlow;
}

int MainWindow::minFlow(const QVector<Station>& stations)
{
    int minFlow = 0;
    for (const Station s : stations)
        minFlow = qMin(minFlow, s.flow);

    return minFlow;
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

    // TODO : not implemented functionalities so widgets are disabled
    m_view->frame_period->setEnabled(false);
    m_view->checkBox_showDuration->setEnabled(false);
}



void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    qDebug() << "onDataLoaded" << "Trip number" << trips.count() << "Station number" << stations.size();
    m_shouldEnableControls = true;
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, stations, m_stationsFilterParams));

    m_tripsFilterParams.maxDistance = maxDistance(trips);
    m_tripsFilterParams.minDistance = minDistance(trips);
    m_tripsFilterParams.maxDuration = maxDuration(trips);
    m_tripsFilterParams.minDuration = minDuration(trips);

    m_stationsFilterParams.maxFlow = maxFlow(stations);
    m_stationsFilterParams.minFlow = minFlow(stations);

    QObject* const distanceRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_distance->rootObject());
    distanceRangeSlider->setProperty("from", m_tripsFilterParams.minDirection);
    distanceRangeSlider->setProperty("to", m_tripsFilterParams.maxDirection);

    QObject* const durationRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_duration->rootObject());
    durationRangeSlider->setProperty("from", m_tripsFilterParams.minDuration);
    durationRangeSlider->setProperty("to", m_tripsFilterParams.maxDuration);

    QObject* const odFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_odFlow->rootObject());
    odFlowRangeSlider->setProperty("from", m_stationsFilterParams.minFlow);
    odFlowRangeSlider->setProperty("to", m_stationsFilterParams.maxFlow);

    QVector<QDate> d = dates(trips);
    d.count();
}

void MainWindow::onFailedToLoadData(const QString& filename, const QString& errorDesc)
{
    Q_UNUSED(filename);
    Q_UNUSED(errorDesc);
    // TODO : onFailedToLoadData
}

void MainWindow::onDataUnloaded()
{
    m_shouldEnableControls = false;
}



void MainWindow::onTripsChanged(const QVector<bss::tripId>& trips)
{
    Q_UNUSED(trips);
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, m_stationsIds, m_tripsDisplayParams));
}

void MainWindow::onSelectionChanged(const QVector<bss::tripId>& selection)
{
    if (selection.isEmpty())
        drawStationsOnMap(m_stationsIds);
    else
        runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawSelectionOnMap, selection));
}

void MainWindow::onStationsOrderChanged(const QVector<bss::stationId>& stations)
{
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, stations, m_tripsDisplayParams));
    if (m_selection.isEmpty())
        drawStationsOnMap(stations);
}

void MainWindow::onHighlightChanged(const bss::stationId& highlight)
{
    Q_UNUSED(highlight);
    // TODO : SEB onHighlightChanged
}

void MainWindow::onTripsDisplayParamsChanged(const TripsDisplayParams &params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, m_stationsIds, params));
}

void MainWindow::onTripsFilterParamsChanged(const TripsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, m_model->trips(), params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortParam& param)
{
    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, stations(m_stationsIds), param));
}

void MainWindow::onStationsFilterParamsChanged(const StationsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, stations(m_stationsIds), params));
}

void MainWindow::onMatrixSelectionChanged(const int& startHour, const int& endHour,
                                          const int& startStationIndex, const int& endStationIndex)
{
    runAsync(QtConcurrent::run(this, &MainWindow::selectTrips, startHour, endHour, startStationIndex, endStationIndex));
}



void MainWindow::onReadyToDrawSelectionOnMap(const QVector<bss::tripId>& arrivalsIds,
                                             const QVector<bss::tripId>& departuresIds,
                                             const QVector<bss::tripId>& cyclesIds)
{
    drawSelectedTripsOnMap(arrivalsIds, departuresIds, cyclesIds);
}

void MainWindow::onReadyToDrawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                                            const QVector<QVector<bss::tripId>>& departuresIds,
                                            const QVector<QVector<bss::tripId>>& cyclesIds,
                                            const bool& showDistance)
{
    drawTripsOnMatrix(arrivalsIds, departuresIds, cyclesIds, showDistance);
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
    runAsync(QtConcurrent::run(this, &MainWindow::unloadData));
}

void MainWindow::on_comboBox_period_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // TODO : SEB on_comboBox_period_currentIndexChanged
}

void MainWindow::on_lineEdit_day_editingFinished()
{
    // TODO : SEB on_lineEdit_day_editingFinished
}

void MainWindow::on_comboBox_dayOfWeek_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // TODO : SEB on_comboBox_dayOfWeek_currentIndexChanged
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

void MainWindow::on_checkBox_showDuration_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    // TODO : SEB on_checkBox_showDuration_stateChanged
    // (aucune idée de ce qu'il faut faire)
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

void MainWindow::on_rangeSlider_odFlow_firstValueChanged(qreal v)
{
    m_view->lineEdit_minOdFlow->setText(QString::number(v));
    m_stationsFilterParams.minFlow = v;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}

void MainWindow::on_rangeSlider_odFlow_secondValueChanged(qreal v)
{
    m_view->lineEdit_maxOdFlow->setText(QString::number(v));
    m_stationsFilterParams.maxFlow = v;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}
