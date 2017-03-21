#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QtMath>
#include <QPoint>

#include "trip.h"
#include "station.h"
#include "config.h"


const QVector<bss::SortParam> MainWindow::SORT_PARAMS = QVector<bss::SortParam>(
{ bss::DISTANCE, bss::DURATION, bss::ARRIVALS, bss::DEPARTURES, bss::CYCLES });

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
    connect(this, &MainWindow::tripsChanged, this, &MainWindow::onTripsChanged, connectionType);
    connect(this, &MainWindow::readyToDrawSelectionOnMap, this, &MainWindow::onReadyToDrawSelectionOnMap, connectionType);
    connect(this, &MainWindow::readyToDrawTripsOnMatrix, this, &MainWindow::onReadyToDrawTripsOnMatrix, connectionType);

    ui->setupUi(this);
    ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->actionClose_all->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
    ui->widget_bottom->hide();

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    connect(ui->actionClose_all, &QAction::triggered, this, &MainWindow::on_action_closeAll_triggered);

    m_model = new Model();

    m_asyncTaskMonitor = new QFutureWatcher<void>();
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted, connectionType);
    connect(m_asyncTaskMonitor, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished, connectionType);

    const QObject* distanceRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_distance->rootObject());
    connect(distanceRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_distance_firstValueChanged(qreal)));
    connect(distanceRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_distance_secondValueChanged(qreal)));

    const QObject* durationRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_duration->rootObject());
    connect(durationRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_duration_firstValueChanged(qreal)));
    connect(durationRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_duration_secondValueChanged(qreal)));

    const QObject* odFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_odFlow->rootObject());
    connect(odFlowRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_odFlow_firstValueChanged(qreal)));
    connect(odFlowRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_odFlow_secondValueChanged(qreal)));

    QObject* const directionRangeSlider = reinterpret_cast<QObject*>((QObject*)ui->rangeSlider_direction->rootObject());
    connect(directionRangeSlider, SIGNAL(firstValueChanged(qreal)), SLOT(on_rangeSlider_direction_firstValueChanged(qreal)));
    connect(directionRangeSlider, SIGNAL(secondValueChanged(qreal)), SLOT(on_rangeSlider_direction_secondValueChanged(qreal)));
    directionRangeSlider->setProperty("from", 0);
    directionRangeSlider->setProperty("to", 360);

    // TODO : SEB m_tripsFilterParams
    /*m_tripsFilterParams.period
    m_tripsFilterParams.day = QDate() ui->lineEdit_day->*/
    m_tripsFilterParams.maxDirection = 360;
    m_tripsFilterParams.minDirection = 0;
    m_tripsFilterParams.maxDistance = 100000;
    m_tripsFilterParams.minDistance = 0;
    m_tripsFilterParams.maxDuration = 100000;
    m_tripsFilterParams.minDuration = 0;

    m_tripsDisplayParams.shouldShowArrivals = ui->checkBox_showArrivals->isChecked();
    m_tripsDisplayParams.shouldShowCycles = ui->checkBox_showCycles->isChecked();
    m_tripsDisplayParams.shouldShowDepartures = ui->checkBox_showDepartures->isChecked();
    m_tripsDisplayParams.shouldShowDistance = ui->checkBox_showDistance->isChecked();

    m_stationsFilterParams.maxOriginDestinationFlow = 100000;
    m_stationsFilterParams.minOriginDestinationFlow = 0;

    m_stationsSortParam = SORT_PARAMS.at(ui->comboBox_order->currentIndex());
}

MainWindow::~MainWindow()
{
    delete m_asyncTaskMonitor;
    delete m_model;
    delete ui;
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
    m_asyncTaskMonitor->setFuture(future);
}



bool MainWindow::loadData(const QStringList& filenames)
{
    bool hasLoadedData = false;
    for (const QString filename : filenames)
    {
        const DataLoadResult loadResult = m_model->loadData(filename);
        if (!loadResult.info.ok)
            emit failedToLoadData(filename, loadResult.info.errorString);
        else
        {
            hasLoadedData = true;
            emit dataLoaded(loadResult.trips, loadResult.stations);
        }
    }
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



void MainWindow::filterTrips(const TripsFilterParams& params)
{
    const TripsFilter filter = TripsFilter(params);
    const QVector<Trip> filteredTrips = filter.filter(m_model->constTrips());
    bss::tripId (*returnId)(const Trip& t) = [](const Trip& t) { return t.id; };
    const QVector<bss::tripId> tripsIds = QtConcurrent::blockingMapped(filteredTrips, returnId);
    if (m_tripsIds != tripsIds)
    {
        m_tripsIds = tripsIds;
        emit tripsChanged(tripsIds);
    }
}

void MainWindow::filterStations(const StationsFilterParams& params)
{
    const StationsFilter filter = StationsFilter(params);
    QVector<Station> filteredStations = filter.filter(m_model->constStations());
    bss::stationId (*returnId)(const Station& s) = [](const Station& s) { return s.id; };
    const QVector<bss::stationId> stationsIds = QtConcurrent::blockingMapped(filteredStations, returnId);
    if (m_stationsIds.toList().toSet() != stationsIds.toList().toSet())
        sortStations(filteredStations, m_stationsSortParam);
}

void MainWindow::sortStations(QVector<Station>& stations, const bss::SortParam& param)
{
    const StationsSorter sorter = StationsSorter(param);
    sorter.sort(stations);

    bss::stationId (*returnId)(const Station& s) = [](const Station& s) { return s.id; };
    const QVector<bss::stationId> stationsOrder = QtConcurrent::blockingMapped(stations, returnId);
    if (m_stationsIds != stationsOrder)
    {
        m_stationsIds = stationsOrder;
        emit stationsOrderChanged(stationsOrder);
    }
}



void MainWindow::prepareToDrawSelectionOnMap(const QVector<bss::tripId>& selection)
{
    // TODO : to optimize if possible
    QVector<bss::tripId> arrivalsId;
    QVector<bss::tripId> departuresId;
    QVector<bss::tripId> cyclesId;

    for (const bss::tripId tId : selection)
    {
        for (const bss::stationId sId : m_stationsIds)
        {
            const Trip t = m_model->trip(sId);
            if (t.isCyclic && m_tripsDisplayParams.shouldShowCycles)
                cyclesId.append(t.id);
            else
            {
                if ((sId == t.startStationId) && m_tripsDisplayParams.shouldShowDistance)
                    departuresId.append(t.id);
                else if (m_tripsDisplayParams.shouldShowArrivals)
                    arrivalsId.append(t.id);
            }
        }
    }

    arrivalsId.squeeze();
    departuresId.squeeze();
    cyclesId.squeeze();

    emit readyToDrawSelectionOnMap(arrivalsId, departuresId, cyclesId);
}

void MainWindow::prepareToDrawTripsOnMatrix(const QVector<bss::tripId>& trips)
{
    // TODO : to optimize if possible
    const int glyphsCount = bss::NB_OF_HOURS * m_stationsIds.count();

    QVector<QVector<bss::tripId>> arrivalsIds;
    QVector<QVector<bss::tripId>> departuresIds;
    QVector<QVector<bss::tripId>> cyclesIds;

    arrivalsIds.reserve(glyphsCount);
    departuresIds.reserve(glyphsCount);
    cyclesIds.reserve(glyphsCount);

    for (int stationIndex = 0; stationIndex < m_stationsIds.size(); ++stationIndex)
    {
        const bss::stationId sId = m_stationsIds.at(stationIndex);
        const Station s = m_model->station(sId);
        for (int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
        {
            const int glyphIndex = (stationIndex * bss::NB_OF_HOURS) + hour;
            const auto filter = [this, &hour](const bss::tripId& id)
            { return (m_model->trip(id).startDateTime.time().hour() == hour); };

            if (m_tripsDisplayParams.shouldShowArrivals)
                arrivalsIds += QtConcurrent::blockingFiltered(s.arrivalsIds, filter);
            else
                arrivalsIds += QVector<bss::tripId>();

            if (m_tripsDisplayParams.shouldShowDepartures)
                departuresIds += QtConcurrent::blockingFiltered(s.departuresIds, filter);
            else
                departuresIds += QVector<bss::tripId>();

            if (m_tripsDisplayParams.shouldShowCycles)
                cyclesIds += QtConcurrent::blockingFiltered(s.cyclesIds, filter);
            else
                cyclesIds += QVector<bss::tripId>();

            arrivalsIds[glyphIndex].squeeze();
            departuresIds[glyphIndex].squeeze();
            cyclesIds[glyphIndex].squeeze();
        }
    }

    arrivalsIds.squeeze();
    departuresIds.squeeze();
    cyclesIds.squeeze();

    emit readyToDrawTripsOnMatrix(arrivalsIds, departuresIds, cyclesIds, m_tripsDisplayParams.shouldShowDistance);
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

    // TODO: DAMIEN: refactoriser DRY
    const float maxLongitude = 180.f;
    const float maxLatitude = 90.f;

    for (const bss::stationId id : stationsIds)
    {
        const Station s = m_model->station(id);
        stationsVertices.append(s.longitude / maxLongitude);
        stationsVertices.append(s.latitude / maxLatitude);

        stationsVertices += QVector<float>({ 1.f, 1.f, 0.3f });
    }

    ui->mapwidget->loadStationsData(stationsVertices, stationsVerticesCount);
    ui->mapwidget->centerView(stationsVertices);
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

    // TODO: DAMIEN: refactoriser DRY
    const float maxLongitude = 180.f;
    const float maxLatitude = 90.f;


    // TODO: DAMIEN : refactoriser DRY
    for (const bss::tripId arrivalId : arrivalsIds)
    {
        const Trip trip = m_model->trip(arrivalId);

        const bss::stationId startStationId = trip.startStationId;
        const Station startStation = m_model->station(startStationId);

        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        tripsVertices += bss::ARRIVAL_ORIGIN_COLOR;


        const bss::stationId endStationId = trip.endStationId;
        const Station endStation = m_model->station(endStationId);

        tripsVertices.append((float) (endStation.longitude / maxLongitude));
        tripsVertices.append((float) (endStation.latitude / maxLatitude));

        tripsVertices += bss::ARRIVAL_DESTINATION_COLOR;

        tripsVertices += 2;
    }

    for (const bss::tripId departureId : departuresIds)
    {
        const Trip trip = m_model->trip(departureId);

        const bss::stationId startStationId = trip.startStationId;
        const Station startStation = m_model->station(startStationId);

        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        tripsVertices += bss::DEPARTURE_ORIGIN_COLOR;


        const bss::stationId endStationId = trip.endStationId;
        const Station endStation = m_model->station(endStationId);

        tripsVertices.append((float) (endStation.longitude / maxLongitude));
        tripsVertices.append((float) (endStation.latitude / maxLatitude));

        tripsVertices += bss::DEPARTURE_DESTINATION_COLOR;

        tripsVertices += 2;
    }

    ui->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
    ui->mapwidget->centerView(tripsVertices);
}

// affiche sur la matrice les trajets nouvellement filtres
void MainWindow::drawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                                   const QVector<QVector<bss::tripId>>& departuresIds,
                                   const QVector<QVector<bss::tripId>>& cyclesIds,
                                   const bool& showDistance)
{
    // x and y;
    const unsigned short positionTupleSize = 2;
    // 1 trip has 2 points (start/end)
    const unsigned short pointPerTrip = 2;
    // R, G, B
    const unsigned short colorTupleSize = 3;

    const float width = ui->timelinematrixwidget->width();
    const float heigth = ui->timelinematrixwidget->height();
    const float intervalX = ui->timelinematrixwidget->width() / (bss::NB_OF_HOURS * 1.f);
    const int glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

    QVector<float> glyphVertices;
    // TODO: DAMIEN: recalculer glyphVertices.reserve();

    int verticesCount = 0;

    // TOOD: DAMIEN : Factoriser faire des fonctions

    int stationIndex;
    stationIndex = 0;
    for (const QVector<bss::tripId> arrivals : arrivalsIds)
    {
        for (unsigned int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
        {
            for (const bss::tripId id : arrivals)
            {
                const Trip t = m_model->trip(id);

                // VERTEX #1
                float positionX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                float positionY = stationIndex * glyphIntervalY;

                float rotationCenterX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                float rotationCenterY = stationIndex * glyphIntervalY + bss::GLYPH_HEIGHT / 2;

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

                glyphVertices += QVector<float>({ positionX, -positionY });
                glyphVertices += bss::GLYPH_ARRIVAL_COLOR;


                // VERTEX #2
                positionX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                positionY = stationIndex * glyphIntervalY + bss::GLYPH_HEIGHT;

                newP = rotatePointAroundTarget(tripDirection,
                                               QPoint(positionX, positionY),
                                               QPoint(rotationCenterX, rotationCenterY));

                positionX = newP.x() / width * 2 - 1;
                positionY = newP.y() / heigth * 2 - 1;

                glyphVertices += QVector<float>({ positionX, -positionY });
                glyphVertices += bss::GLYPH_ARRIVAL_COLOR;

                verticesCount+= 2;
            }
        }
        stationIndex++;
    }

    stationIndex = 0;
    for (const QVector<bss::tripId> departures : departuresIds)
    {
        for (unsigned int hour = 0; hour < bss::NB_OF_HOURS; ++hour)
        {
            for (const bss::tripId id : departures)
            {
                const Trip t = m_model->trip(id);

                // VERTEX #1
                float positionX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                float positionY = stationIndex * glyphIntervalY;

                float rotationCenterX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                float rotationCenterY = stationIndex * glyphIntervalY + bss::GLYPH_HEIGHT / 2;

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

                glyphVertices += QVector<float>({ positionX, -positionY });
                glyphVertices += bss::GLYPH_DEPARTURE_COLOR;


                // VERTEX #2
                positionX = bss::TIMELINE_OFFSET_X + hour * intervalX;
                positionY = stationIndex * glyphIntervalY + bss::GLYPH_HEIGHT;

                newP = rotatePointAroundTarget(tripDirection,
                                               QPoint(positionX, positionY),
                                               QPoint(rotationCenterX, rotationCenterY));

                positionX = newP.x() / width * 2 - 1;
                positionY = newP.y() / heigth * 2 - 1;

                glyphVertices += QVector<float>({ positionX, -positionY });
                glyphVertices += bss::GLYPH_DEPARTURE_COLOR;

                verticesCount+= 2;
            }
        }
        stationIndex++;
    }

    ui->timelinematrixwidget->loadGlyphsData(glyphVertices, verticesCount);
}



void MainWindow::onAsyncTaskStarted()
{
    m_canApplicationExit = false;
    ui->menuBar->setEnabled(false);
    ui->frame_controls->setEnabled(false);
}

void MainWindow::onAsyncTaskFinished()
{
    m_canApplicationExit = true;
    ui->menuBar->setEnabled(m_shouldEnableMenuBar);
    ui->frame_controls->setEnabled(m_shouldEnableControls);
}



void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    qDebug() << "onDataLoaded" << "Trip number" << trips.size() << "Station number" << stations.size();
    m_shouldEnableControls = true;
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, m_stationsFilterParams));
}

void MainWindow::onFailedToLoadData(const QString& filename, const QString& errorDesc)
{
    // TODO : onFailedToLoadData
}

void MainWindow::onDataUnloaded()
{
    m_shouldEnableControls = false;
}



void MainWindow::onTripsChanged(const QVector<bss::tripId>& trips)
{
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, trips));
}

void MainWindow::onSelectionChanged(const QVector<bss::tripId>& selection)
{
    if (selection.isEmpty())
        drawStationsOnMap(m_stationsIds);
    else
        runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawSelectionOnMap, selection));
}

void MainWindow::onStationsOrderChanged(const QVector<bss::stationId>& stationsOrder)
{
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, m_tripsIds));
    if (m_selection.isEmpty())
        drawStationsOnMap(m_stationsIds);
    else
        runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawSelectionOnMap, m_tripsIds));
}

void MainWindow::onHighlightChanged(const bss::stationId& highlight)
{
    // TODO : SEB onHighlightChanged
}

void MainWindow::onTripsDisplayParamsChanged(const TripsDisplayParams &params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, m_tripsIds));
    drawStationsOnMap(m_stationsIds);
}

void MainWindow::onTripsFilterParamsChanged(const TripsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortParam& param)
{
    // TODO : to optimize if possible
    QVector<Station> stationsToSort;
    stationsToSort.reserve(m_stationsIds.size());
    for (const bss::stationId id : m_stationsIds)
        stationsToSort.append(m_model->station(id));

    stationsToSort.squeeze();

    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, stationsToSort, param));
}

void MainWindow::onStationsFilterParamsChanged(const StationsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, params));
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
    runAsync(QtConcurrent::run(this, &MainWindow::loadData, filenames));
}

void MainWindow::on_action_closeAll_triggered()
{
    runAsync(QtConcurrent::run(this, &MainWindow::unloadData));
}

void MainWindow::on_comboBox_period_currentIndexChanged(int index)
{
    // TODO : SEB on_comboBox_period_currentIndexChanged
}

void MainWindow::on_lineEdit_day_editingFinished()
{
    // TODO : SEB on_lineEdit_day_editingFinished
}

void MainWindow::on_comboBox_dayOfWeek_currentIndexChanged(int index)
{
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
    ui->lineEdit_minDistance->setText(QString::number(v));
    m_tripsFilterParams.minDistance = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_distance_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDistance->setText(QString::number(v));
    m_tripsFilterParams.maxDistance = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_duration_firstValueChanged(qreal v)
{
    ui->lineEdit_minDuration->setText(QString::number(v));
    m_tripsFilterParams.minDuration = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_duration_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDuration->setText(QString::number(v));
    m_tripsFilterParams.maxDuration = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_direction_firstValueChanged(qreal v)
{
    ui->lineEdit_minDirection->setText(QString::number(v));
    m_tripsFilterParams.minDirection = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_direction_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDirection->setText(QString::number(v));
    m_tripsFilterParams.maxDirection = v;
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_rangeSlider_odFlow_firstValueChanged(qreal v)
{
    ui->lineEdit_minOdFlow->setText(QString::number(v));
    m_stationsFilterParams.minOriginDestinationFlow = v;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}

void MainWindow::on_rangeSlider_odFlow_secondValueChanged(qreal v)
{
    ui->lineEdit_maxOdFlow->setText(QString::number(v));
    m_stationsFilterParams.maxOriginDestinationFlow = v;
    onStationsFilterParamsChanged(m_stationsFilterParams);
}
