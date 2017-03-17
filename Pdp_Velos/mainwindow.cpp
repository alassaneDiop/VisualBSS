#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>

#include "config.h"


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /* If the signal is queued, the parameters must be of types that are known to Qt's meta-object system,
     * because Qt needs to copy the arguments to store them in an event behind the scenes.
     * */
    qRegisterMetaType<QVector<Trip>>("QVector<Trip>");
    qRegisterMetaType<QVector<Station>>("QVector<Station>");

    const Qt::ConnectionType connectionType = (Qt::ConnectionType)(Qt::QueuedConnection | Qt::UniqueConnection);
    connect(this, &MainWindow::dataLoaded, this, &MainWindow::onDataLoaded, connectionType);
    connect(this, &MainWindow::failedToLoadData, this, &MainWindow::onFailedToLoadData, connectionType);
    connect(this, &MainWindow::dataUnloaded, this, &MainWindow::onDataUnloaded, connectionType);

    ui->setupUi(this);
    ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->actionClose_all->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
    ui->widget_bottom->hide();

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    connect(ui->actionClose_all, &QAction::triggered, this, &MainWindow::on_action_closeAll_triggered);

    m_model = new Model();

    m_futureWatcher = new QFutureWatcher<void>();
    connect(m_futureWatcher, &QFutureWatcher<void>::started, this, &MainWindow::onAsyncTaskStarted, connectionType);
    connect(m_futureWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAsyncTaskFinished, connectionType);

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
    directionRangeSlider->setProperty("snapMode", "NoSnap");
    directionRangeSlider->setProperty("stepSize", 0.01);
    directionRangeSlider->setProperty("from", 0);
    directionRangeSlider->setProperty("to", 360);
}

MainWindow::~MainWindow()
{
    delete m_futureWatcher;
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
    m_futureWatcher->setFuture(future);
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
    const QVector<bss::tripId> filteredTripsIds = QtConcurrent::blockingMapped(filteredTrips, returnId);
    if (m_filteredTripsIds != filteredTripsIds)
    {
        m_filteredTripsIds = filteredTripsIds;
        onFilteredTripsChanged(filteredTripsIds);
    }
}

void MainWindow::filterStations(const StationsFilterParams& params)
{
    const StationsFilter filter = StationsFilter(params);
    QVector<Station> filteredStations = filter.filter(m_model->constStations());
    bss::stationId (*returnId)(const Station& s) = [](const Station& s) { return s.id; };
    const QVector<bss::stationId> filteredStationsIds = QtConcurrent::blockingMapped(filteredStations, returnId);
    if (QSet<bss::stationId>::fromList(m_orderedStationsIds.toList()) != QSet<bss::stationId>::fromList(filteredStationsIds.toList()))
        sortStations(m_sortOrder, filteredStations);
}

void MainWindow::sortStations(const bss::SortOrder& param, QVector<Station>& stations)
{
    const StationsSorter sorter = StationsSorter(param);
    sorter.sort(stations);

    bss::stationId (*returnId)(const Station& s) = [](const Station& s) { return s.id; };
    const QVector<bss::stationId> stationsOrder = QtConcurrent::blockingMapped(stations, returnId);
    if (m_orderedStationsIds != stationsOrder)
    {
        m_orderedStationsIds = stationsOrder;
        onStationsOrderChanged(stationsOrder);
    }
}



void MainWindow::drawMap(const QVector<Station>& stations, const QVector<Trip>& arrivals, const QVector<Trip>& departures, const QVector<Trip>& cycles)
{
    // TODO : Damien : drawStations
}

void MainWindow::drawTimelineMatrix(const QVector<Trip>& arrivals, const QVector<Trip>& departures, const QVector<Trip>& cycles)
{
    // TODO : Damien : drawTrips
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
    ui->frame_controls->setEnabled(m_shouldEnableControlsFrame);
}



void MainWindow::onDataLoaded(const QVector<Trip>& trips, const QVector<Station>& stations)
{
    m_shouldEnableControlsFrame = true;
    filterStations(m_stationsFilterParams);

    qDebug() << "onDataLoaded" << "Trip number" << trips.size() << "Station number" << stations.size();

    // x and y;
    const unsigned short positionTupleSize = 2;

    // 1 trip has 2 points (start/end)
    const unsigned short pointPerTrip = 2;

    // R, G, B
    const unsigned short colorTupleSize = 3;

    QVector<float> tripsVertices;
    tripsVertices.reserve((trips.size() * positionTupleSize + trips.size() * colorTupleSize) * pointPerTrip);
    const int tripsVerticesCount = trips.size() * pointPerTrip;

    const float maxLongitude = 180.f;
    const float maxLatitude = 90.f;

    for (const Trip t : trips)
    {
        int startStationId = t.startStationId;
        const Station startStation = m_model->constStation(startStationId);

        //        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        //        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        tripsVertices.append((float) (startStation.longitude / maxLongitude));
        tripsVertices.append((float) (startStation.latitude / maxLatitude));

        // incoming
        // red origin           1, 0, 0
        // yellow destination   1, 1, 0

        // outgoing
        // cyan origin          0, 1, 1
        // blue end             0, 0, 1
        tripsVertices.append(1.f);
        tripsVertices.append(0.f);
        tripsVertices.append(0.f);

        int endStationId = t.endStationId;
        const Station endStation = m_model->constStation(endStationId);

        tripsVertices.append((float) (endStation.longitude / maxLongitude));
        tripsVertices.append((float) (endStation.latitude / maxLatitude));

        tripsVertices.append(0.f);
        tripsVertices.append(0.f);
        tripsVertices.append(1.f);
    }

    QVector<float> stationsVertices;
    const int stationsVerticesCount = stations.size();
    stationsVertices.reserve(stationsVerticesCount * positionTupleSize + stationsVerticesCount * colorTupleSize);

    for (const Station s : stations)
    {
        stationsVertices.append(s.longitude / maxLongitude);
        stationsVertices.append(s.latitude / maxLatitude);

        stationsVertices.append(1.f);
        stationsVertices.append(1.f);
        stationsVertices.append(.3f);
    }

    // TEST D'AFFICHAGE DES GLYPHS
    // LARGEUR ET HAUTEUR
    const float width = ui->timelinematrixwidget->width();
    const float heigth = ui->timelinematrixwidget->height();
    const int numberOfStations = stations.size();

    // TODO: appeler un methode de timeline pour recuperer la valeur de numberOfHour
    const float intervalX = ui->timelinematrixwidget->width() / (bss::NB_OF_HOUR * 1.f);

    const int glyphHeight = bss::GLYPH_HEIGHT;
    const int glyphSpaceBetweenToLines = bss::SPACE_BETWEEN_GLYPHS;
    const int glyphIntervalY = glyphHeight + glyphSpaceBetweenToLines;

    QVector<float> glyphVertices;
    for (int i = 0; i < numberOfStations; ++i)
    {
        for (int j = 0; j < bss::NB_OF_HOUR; ++j)
        {
            for (int k = 0; k < 1; k++)
            {
                // JUST DRAWING LINE
                // VERTEX #1
                float positionX = bss::TIMELINE_OFFSET_X + j * intervalX;
                float positionY = i * glyphIntervalY;

                //float positionXT =

                positionX = positionX / width * 2 - 1;
                positionY = positionY / heigth * 2 - 1;

                glyphVertices.append(positionX);
                glyphVertices.append(-positionY);

                // VERTEX #2
                positionX = bss::TIMELINE_OFFSET_X + j * intervalX;
                positionY = i * glyphIntervalY + glyphHeight;

                positionX = positionX / width * 2 - 1;
                positionY = positionY / heigth * 2 - 1;

                glyphVertices.append(positionX);
                glyphVertices.append(-positionY);
            }
        }
    }

    ui->mapwidget->loadStationsData(stationsVertices, stationsVerticesCount);
    ui->mapwidget->loadTripsData(tripsVertices, tripsVerticesCount);
    ui->mapwidget->centerView(stationsVertices);
    ui->timelinematrixwidget->loadGlyphsData(glyphVertices, 2 * bss::NB_OF_HOUR * numberOfStations);
}

void MainWindow::onFailedToLoadData(const QString& filename, const QString& errorDesc)
{
    // TODO : onFailedToLoadData
}

void MainWindow::onDataUnloaded()
{
    m_shouldEnableControlsFrame = false;
}



void MainWindow::onFilteredTripsChanged(const QVector<bss::tripId>& filteredTrips)
{
    // TODO : SEB onFilteredTripsChanged
    QVector<Station> stations;
    QVector<Trip> arrivals, departures, cycles;

    for (const bss::stationId& stationId : m_orderedStationsIds)
    {
        const Station s = m_model->constStation(stationId);
        stations.append(s);

        for (const bss::tripId& id : s.arrivalsIds)
            arrivals.append(m_model->constTrip(id));

        for (const bss::tripId& id : s.departuresIds)
            departures.append(m_model->constTrip(id));

        for (const bss::tripId& id : s.cyclesIds)
            cycles.append(m_model->constTrip(id));
    }

    drawMap(stations, arrivals, departures, cycles);
    drawTimelineMatrix(arrivals, departures, cycles);
}

void MainWindow::onSelectionChanged(const QVector<bss::tripId>& selection)
{
    // TODO : SEB onSelectionChanged
    /*QVector<Trip> trips;
    for (const bss::tripId& id : selection)
        trips.append(m_model->constTrip(id));

    drawTimelineMatrix(arrivals, departures, cycles);*/
}

void MainWindow::onStationsOrderChanged(const QVector<bss::stationId>& stationsOrder)
{
    // TODO : SEB onStationsOrderChanged
    QVector<Station> stations;
    QVector<Trip> arrivals, departures, cycles;

    for (const bss::stationId& stationId : stationsOrder)
    {
        const Station s = m_model->constStation(stationId);
        stations.append(s);

        for (const bss::tripId& id : s.arrivalsIds)
            arrivals.append(m_model->constTrip(id));

        for (const bss::tripId& id : s.departuresIds)
            departures.append(m_model->constTrip(id));

        for (const bss::tripId& id : s.cyclesIds)
            cycles.append(m_model->constTrip(id));
    }

    drawMap(stations, arrivals, departures, cycles);
    drawTimelineMatrix(arrivals, departures, cycles);
}

void MainWindow::onHighlightChanged(const bss::stationId& highlight)
{
    // TODO : onHighlightChanged
}

void MainWindow::onTripsFilterParamsChanged(const TripsFilterParams& params)
{
    runAsync(QtConcurrent::run(this, &MainWindow::filterTrips, params));
}

void MainWindow::onStationsSorterParamChanged(const bss::SortOrder& param)
{
    QVector<Station> stationsToSort;
    stationsToSort.reserve(m_orderedStationsIds.size());
    for (const bss::stationId id : m_orderedStationsIds)
        stationsToSort.append(m_model->station(id));

    runAsync(QtConcurrent::run(this, &MainWindow::sortStations, param, stationsToSort));
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
    // TODO : on_comboBox_period_currentIndexChanged
}

void MainWindow::on_lineEdit_day_editingFinished()
{
    // TODO : on_lineEdit_day_editingFinished
}

void MainWindow::on_comboBox_dayOfWeek_currentIndexChanged(int index)
{
    // TODO : on_comboBox_dayOfWeek_currentIndexChanged
}

void MainWindow::on_checkBox_arrivals_stateChanged(int arg1)
{
    m_tripsFilterParams.shouldShowArrivals = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_checkBox_departures_stateChanged(int arg1)
{
    m_tripsFilterParams.shouldShowDepartures = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_checkBox_cycles_stateChanged(int arg1)
{
    m_tripsFilterParams.shouldShowCycles = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_checkBox_duration_stateChanged(int arg1)
{
    // TODO : on_checkBox_duration_stateChanged
}

void MainWindow::on_checkBox_distance_stateChanged(int arg1)
{
    m_tripsFilterParams.shouldShowDistance = (arg1 == Qt::Checked);
    onTripsFilterParamsChanged(m_tripsFilterParams);
}

void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    // order corresponds to UI's combobox
    const QVector<bss::SortOrder> sortParams = QVector<bss::SortOrder>(
    { bss::DISTANCE, bss::DURATION, bss::ARRIVALS, bss::DEPARTURES, bss::CYCLES });
    m_sortOrder = sortParams.at(index);
    onStationsSorterParamChanged(m_sortOrder);
}

void MainWindow::on_rangeSlider_distance_firstValueChanged(qreal v)
{
    ui->lineEdit_minDistance->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_distance_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDistance->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_duration_firstValueChanged(qreal v)
{
    ui->lineEdit_minDuration->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_duration_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDuration->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_direction_firstValueChanged(qreal v)
{
    ui->lineEdit_minDirection->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_direction_secondValueChanged(qreal v)
{
    ui->lineEdit_maxDirection->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_odFlow_firstValueChanged(qreal v)
{
    ui->lineEdit_minOdFlow->setText(QString::number(v));
}

void MainWindow::on_rangeSlider_odFlow_secondValueChanged(qreal v)
{
    ui->lineEdit_maxOdFlow->setText(QString::number(v));
}
