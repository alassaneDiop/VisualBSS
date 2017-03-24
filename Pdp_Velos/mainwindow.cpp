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
    connect(this, &MainWindow::stationsOrderChanged, this, &MainWindow::onStationsOrderChanged, connectionType);
    connect(this, &MainWindow::selectionChanged, this, &MainWindow::onSelectionChanged, connectionType);
    connect(this, &MainWindow::tripsChanged, this, &MainWindow::onTripsChanged, connectionType);
    connect(this, &MainWindow::readyToDrawSelectionOnMap, this, &MainWindow::onReadyToDrawSelectionOnMap, connectionType);
    connect(this, &MainWindow::readyToDrawTripsOnMatrix, this, &MainWindow::onReadyToDrawTripsOnMatrix, connectionType);

    m_view->setupUi(this);
    m_view->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_view->actionClose_all->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));

    m_view->listView_top->setStyleSheet("QListView::item:hover{background-color:#a8f21f;}");
    m_view->listView_last->setStyleSheet("QListView::item:hover{background-color:#a8f21f;}");

    connect(m_view->actionOpen, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    connect(m_view->actionClose_all, &QAction::triggered, this, &MainWindow::on_action_closeAll_triggered);

    connect(m_view->timelinematrixwidget, &MatrixGLWidget::selectionChanged, this, &MainWindow::onMatrixSelectionChanged);

    m_model = new Model();
    connect(m_model, &Model::dataLoaded, this, &MainWindow::onDataLoaded, connectionType);
    connect(m_model, &Model::failedToLoadData, this, &MainWindow::onFailedToLoadData, connectionType);
    connect(m_model, &Model::dataUnloaded, this, &MainWindow::onDataUnloaded, connectionType);

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



bool MainWindow::loadDataFromFiles(const QStringList& filenames, const bool& parallel)
{
    bool hasLoadedData = false;
    for (const QString filename : filenames)
        if (m_model->loadData(filename, parallel))
            hasLoadedData = true;

    return hasLoadedData;
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

void MainWindow::selectTrips(const int& fromHour, const int& toHour,
                             const int& fromStationIndex, const int& toStationIndex)
{
    TripsSelectionParams selectionParams;
    selectionParams.fromHour = fromHour;
    selectionParams.toHour = toHour;
    selectionParams.stationsIndices = m_stationsIds.mid(fromStationIndex, fromStationIndex + toStationIndex);

    const TripsSelector selector = TripsSelector(selectionParams);
    const QVector<bss::tripId> selection = ids(selector.select(m_model->trips(m_tripsIds)));
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

        stationsVertices.append(1.f);
        stationsVertices.append(1.f);
        stationsVertices.append(0.3f);
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
            QVector<float>& tripsVertices,
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
            tripsVertices.append(originColor);


            const bss::stationId endStationId = trip.endStationId;
            const Station endStation = m_model->station(endStationId);

            tripsVertices.append((float) (endStation.longitude / maxLongitude));
            tripsVertices.append((float) (endStation.latitude / maxLatitude));
            tripsVertices.append(destinationColor);

            verticesCount += 2;
        }
    };

    // FIXME: couleur
    drawTrips(arrivalsIds, tripsVertices, tripsVerticesCount, bss::ARRIVAL_ORIGIN_COLOR, bss::ARRIVAL_DESTINATION_COLOR);
    drawTrips(departuresIds, tripsVertices, tripsVerticesCount, bss::DEPARTURE_ORIGIN_COLOR, bss::DEPARTURE_DESTINATION_COLOR);

    m_view->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
    m_view->mapwidget->centerView(tripsVertices);
}



// TODO: Séb : mettre ou tu veux
QVector<float> MainWindow::drawTrip(bss::tripId id, float posX, float posY,
                                    const QVector<float>& color)
{
    const int colorTupleSize = 3;
    const int positionTupleSize = 2;
    const int numberOfPointPerTrip = 2;

    QVector<float> tripData;
    tripData.reserve((colorTupleSize + positionTupleSize) * numberOfPointPerTrip);

    // VERTEX #1
    const float positionXP1 = posX;
    const float positionYP1 = posY - (bss::GLYPH_HEIGHT / 2.f);

    const float rotationCenterX = posX;
    const float rotationCenterY = posY;

    const auto rotatePointAroundTarget = [](float angle, QPointF position, QPointF target)
    {
        const float sin = qSin(angle);
        const float cos = qCos(angle);

        // Translate positionX/Y to the origin
        float pX = target.x() - position.x();
        float pY = target.y() - position.y();

        // Do the rotation
        float newX = pX * cos - pY * sin;
        float newY = pX * sin + pY * cos;

        // Translate back
        newX += position.x();
        newY += position.y();

        return QPointF(newX, newY);
    };

    const Trip t = m_model->trip(id);
    const float tripDirection = t.direction;

    QPointF positionAfterRotation;
    positionAfterRotation = rotatePointAroundTarget(tripDirection,
                                   QPointF(positionXP1, positionYP1),
                                   QPointF(rotationCenterX, rotationCenterY));

    const float width = m_view->timelinematrixwidget->width();
    const float heigth = m_view->timelinematrixwidget->height();

    const float newPosXP1 = positionAfterRotation.x() / width * 2 - 1;
    const float newPosYP1 = positionAfterRotation.y() / heigth * 2 - 1;

    tripData.append(newPosXP1);
    tripData.append(-newPosYP1);
    tripData.append(color);


    // VERTEX #2
    const float positionXP2 = posX;
    const float positionYP2 = posY + (bss::GLYPH_HEIGHT / 2.f);

    positionAfterRotation = rotatePointAroundTarget(tripDirection,
                                   QPointF(positionXP2, positionYP2),
                                   QPointF(rotationCenterX, rotationCenterY));

    const float newPosXP2 = positionAfterRotation.x() / width * 2 - 1;
    const float newPosYP2 = positionAfterRotation.y() / heigth * 2 - 1;

    tripData.append(newPosXP2);
    tripData.append(-newPosYP2);
    tripData.append(color);

    return tripData;
}

QVector<float> MainWindow::drawGlyph(const QVector<bss::tripId>& trips,
                                     float posX, float posY, const QVector<float>& color)
{
    QVector<float> glyph;
    for (bss::tripId id : trips)
    {
        QVector<float> trip = drawTrip(id, posX, posY, color);
        glyph.append(trip);
    }

    return glyph;
}
// END

// affiche sur la matrice les trajets nouvellement filtres
void MainWindow::drawTripsOnMatrix(const QVector<QVector<bss::tripId>>& arrivalsIds,
                                   const QVector<QVector<bss::tripId>>& departuresIds,
                                   const QVector<QVector<bss::tripId>>& cyclesIds,
                                   const bool& showDistance)
{
    Q_UNUSED(cyclesIds);
    Q_UNUSED(showDistance);

    const float glyphIntervalX = m_view->timelinematrixwidget->width() / (bss::NB_OF_HOURS * 1.f);
    const float glyphIntervalY = bss::GLYPH_HEIGHT + bss::SPACE_BETWEEN_GLYPHS;

    QVector<float> glyphsData;
    // TODO: Damien
    //glyphsData.reserve();

    int i = 0;
    int stationIndex = 0;
    for (const QVector<bss::tripId> ids : arrivalsIds)
    {

        const float posX = (bss::TIMELINE_OFFSET_X + (i * glyphIntervalX));
        const float posY = stationIndex * glyphIntervalY;

        glyphsData.append(drawGlyph(ids, posX, posY, bss::GLYPH_ARRIVAL_COLOR));

        i++;
        i = i % 24;

        // if i equals 0, thats means we move to another station (and a new line to draw glyphs)
        if (i == 0)
            stationIndex++;
    }

    i = 0;
    stationIndex = 0;
    for (const QVector<bss::tripId> ids : departuresIds)
    {

        const float posX = (bss::TIMELINE_OFFSET_X + (i * glyphIntervalX));
        const float posY = stationIndex * glyphIntervalY;

        glyphsData.append(drawGlyph(ids, posX, posY, bss::GLYPH_DEPARTURE_COLOR));

        i++;
        i = i % 24;

        // if i equals 0, thats means we move to another station (and a new line to draw glyphs)
        if (i == 0)
            stationIndex++;
    }

    const int vertexSize = 5; // 2 for position x/y and 3 for color R/G/B
    m_view->timelinematrixwidget->loadGlyphsData(glyphsData, glyphsData.size() / vertexSize);
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

quint64 MainWindow::maxDuration(const QVector<Trip>& trips)
{
    quint64 maxDuration = 0;
    for (const Trip t : trips)
        maxDuration = qMax(maxDuration, t.duration);

    return maxDuration;
}

int MainWindow::maxDistance(const QVector<Trip>& trips)
{
    int maxDistance = 0;
    for (const Trip t : trips)
        maxDistance = qMax(maxDistance, t.distance);

    return maxDistance;
}

int MainWindow::maxFlow(const QVector<Station>& stations)
{
    int maxFlow = 0;
    for (const Station s : stations)
        maxFlow = qMax(maxFlow, s.tripsFlow);

    return maxFlow;
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
    m_stationsFilterParams.maxTripsFlow = maxFlow(stations);

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
    distanceRangeSlider->setProperty("to", m_tripsFilterParams.maxDirection);
    distanceRangeSlider->blockSignals(false);

    QObject* const durationRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_duration->rootObject());
    durationRangeSlider->blockSignals(true);
    durationRangeSlider->setProperty("to", m_tripsFilterParams.maxDuration);
    durationRangeSlider->blockSignals(false);

    QObject* const tripsFlowRangeSlider = reinterpret_cast<QObject*>((QObject*)m_view->rangeSlider_tripsFlow->rootObject());
    tripsFlowRangeSlider->blockSignals(true);
    tripsFlowRangeSlider->setProperty("to", m_stationsFilterParams.maxTripsFlow);
    tripsFlowRangeSlider->blockSignals(false);

    m_view->lineEdit_maxDistance->setText(QString::number(m_tripsFilterParams.maxDirection));
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
    // TODO : clear selection
    runAsync(QtConcurrent::run(this, &MainWindow::prepareToDrawTripsOnMatrix, stations, m_tripsDisplayParams));

    if (m_selection.isEmpty())
        drawStationsOnMap(stations);

    const QVector<bss::stationId> topStationsIds = stations.mid(0, bss::RANK_SIZE);
    QStringList topStationsNames;
    topStationsNames.reserve(topStationsIds.size());
    for (const bss::stationId id : topStationsIds)
        topStationsNames.append(m_model->station(id).name);

    const int lastStart = stations.size() - bss::RANK_SIZE;
    const int lastEnd = stations.size() - 1;
    const QVector<bss::stationId> lastStationsIds = stations.mid(lastStart, lastEnd);
    QStringList lastStationsNames;
    lastStationsNames.reserve(lastStationsIds.size());
    for (auto it = lastStationsIds.rbegin(); it != lastStationsIds.rend(); ++it)
        lastStationsNames.append(m_model->station(*it).name);

    m_view->listView_top->setModel(new QStringListModel(topStationsNames));
    m_view->listView_last->setModel(new QStringListModel(lastStationsNames));
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
    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, m_model->stations(m_stationsIds), param));
}

void MainWindow::onStationsFilterParamsChanged(const StationsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterStations, m_model->stations(m_stationsIds), params));
}

void MainWindow::onMatrixSelectionChanged(const int& fromHour, const int& toHour,
                                          const int& fromStationIndex, const int& toStationIndex)
{
    runAsync(QtConcurrent::run(this, &MainWindow::selectTrips, fromHour, toHour, fromStationIndex, toStationIndex));
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
    runAsync(QtConcurrent::run(m_model, &Model::unloadData));
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
